import os
import re
import json
from typing import Final, List, Set, Dict, Type, Optional

# Constants
VALID_EXTENSIONS: Final[Set[str]] = {".cpp", ".h", ".py", ".ts", ".js", ".rs"}
IGNORED_DIRS: Final[Set[str]] = {"build", "node_modules", ".git", "__pycache__"}

class DependencyExtractor:
    """Abstract base class for dependency extraction."""
    def extract(self, content: str) -> List[str]:
        raise NotImplementedError

class CppExtractor(DependencyExtractor):
    """Extracts dependencies from C++ files."""
    def extract(self, content: str) -> List[str]:
        return re.findall(r'#include\s*["<]([^">]+)[">]', content)

class PythonExtractor(DependencyExtractor):
    """Extracts dependencies from Python files."""
    def extract(self, content: str) -> List[str]:
        imports = []
        
        raw_imports = re.findall(r'^\s*import\s+([\w\.,\s]+)', content, re.MULTILINE)
        for imp_str in raw_imports:
            parts = imp_str.split(',')
            for part in parts:
                name = part.strip().split()[0]
                imports.append(name)

        from_imports = re.findall(r'^\s*from\s+([\w\.]+)\s+import\s+([\w\.,\s]+)', content, re.MULTILINE)
        
        for module, names in from_imports:
            if module == ".":
                parts = names.split(',')
                for part in parts:
                    name = part.strip().split()[0]
                    imports.append(name)
            elif module.startswith("."):
                clean_module = module.lstrip('.')
                imports.append(clean_module)
            else:
                imports.append(module)
                
        return imports

class TypeScriptExtractor(DependencyExtractor):
    """Extracts dependencies from TypeScript/JavaScript files."""
    def extract(self, content: str) -> List[str]:
        deps = []
        deps.extend(re.findall(r'(?:import|export)\s+.*?\s+from\s*[\'"]([^\'"]+)[\'"]', content))
        deps.extend(re.findall(r'import\s*[\'"]([^\'"]+)[\'"]', content))
        deps.extend(re.findall(r'require\s*\(\s*[\'"]([^\'"]+)[\'"]\s*\)', content))
        return deps

class RustExtractor(DependencyExtractor):
    """Extracts dependencies from Rust files."""
    def extract(self, content: str) -> List[str]:
        deps = []
        # mod x;
        deps.extend(re.findall(r'mod\s+(\w+);', content))
        # use x::y;
        uses = re.findall(r'use\s+([\w:]+);', content)
        for u in uses:
            parts = u.split('::')
            if parts:
                deps.append(parts[0])
        return deps

class RepoParser:
    """
    Parses a repository to extract file structure and dependencies.
    """
    def __init__(self, repo_path: str):
        self.current_id = 0
        self.repo_path = repo_path
        self.file_map = {}
        self.source_files = []
        self.dependency_map = {}
        
        self.extractors: Dict[str, DependencyExtractor] = {
            ".cpp": CppExtractor(),
            ".h": CppExtractor(),
            ".py": PythonExtractor(),
            ".ts": TypeScriptExtractor(),
            ".js": TypeScriptExtractor(),
            ".rs": RustExtractor(),
            ".tsx": TypeScriptExtractor(),
            ".jsx": TypeScriptExtractor(),
        }
    
    def walk(self) -> List[str]:
        """
        Walks the repository and identifies source files.
        """
        print(f"Walking {self.repo_path}...")
        for root, dirs, files in os.walk(self.repo_path):
            # Modify dirs in-place to skip ignored directories
            dirs[:] = [d for d in dirs if d not in IGNORED_DIRS]

            for file in files:
                ext = os.path.splitext(file)[1]
                if ext in self.extractors:
                    full_path = os.path.join(root, file)
                    relative_path = os.path.relpath(full_path, self.repo_path)
                    if relative_path not in self.file_map:
                        self.file_map[relative_path] = self.current_id
                        self.current_id += 1
                    self.source_files.append(relative_path)
        
        return self.source_files
    
    def scan(self):
        """
        Scans source files for dependencies.
        """
        for relative_file in self.source_files:
            full_path = os.path.join(self.repo_path, relative_file)
            ext = os.path.splitext(relative_file)[1]
            
            extractor = self.extractors.get(ext)
            if not extractor:
                continue

            try:
                with open(full_path, 'r', encoding="utf-8", errors="ignore") as f:
                    data = f.read()
            except FileNotFoundError:
                print(f"File {relative_file} not found")
                continue
            
            dependencies = extractor.extract(data)
            if dependencies:
                self.dependency_map[relative_file] = dependencies

    def _resolve_dependency(self, source_file: str, dep_str: str) -> Optional[str]:
        """
        Attempts to resolve a dependency string to a file path in the repo.
        """
        source_dir = os.path.dirname(source_file)
        
        # Potential candidates for resolution
        candidates = []
        
        # 1. Exact match (relative to source dir)
        candidates.append(os.path.join(source_dir, dep_str))
        
        # 2. Exact match (relative to repo root - for some imports)
        candidates.append(dep_str)

        # 3. Try extensions if not present
        base, ext = os.path.splitext(dep_str)
        if not ext:
            for valid_ext in self.extractors.keys():
                # relative to source dir
                candidates.append(os.path.join(source_dir, dep_str + valid_ext))
                # relative to source dir, index file (common in JS/TS/Python)
                candidates.append(os.path.join(source_dir, dep_str, "index" + valid_ext))
                candidates.append(os.path.join(source_dir, dep_str, "__init__" + valid_ext))
                candidates.append(os.path.join(source_dir, dep_str, "mod" + valid_ext)) # Rust
                
                # relative to repo root (for absolute imports)
                candidates.append(dep_str + valid_ext)

        for path in candidates:
            # Normalize path to resolve '..' and '.'
            norm_path = os.path.normpath(path)
            
            # Check if this passes boundaries or exists
            if norm_path.startswith(".."):
                continue
                
            if norm_path in self.file_map:
                return norm_path
        
        return None

    def get_graph_data(self):
        """
        Serializes the dependency graph to JSON.
        """
        nodes = []
        edges = []
        for name, id in self.file_map.items():
            nodes.append({
                "id": id,
                "name": name
            })

        for source_file, dependencies in self.dependency_map.items():
            if source_file not in self.file_map:
                continue
            
            source_id = self.file_map[source_file]

            for dep_str in dependencies:
                target_path = self._resolve_dependency(source_file, dep_str)
                
                if target_path:
                    target_id = self.file_map[target_path]
                    edges.append({
                        "source": source_id,
                        "target": target_id
                    })

        output_data = {
            "nodes": nodes,
            "edges": edges
        }
        
        return output_data

        # try:
        #     with open(output_path, "w", encoding="utf-8") as f:
        #         json.dump(output_data, f, indent=4)
        #     print(f"Serialized {len(nodes)} nodes and {len(edges)} edges to {output_path}")
        # except Exception as e:
        #     print(f"Failed to write to {output_path}: {e}")
