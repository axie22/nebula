import os
import re
import json
from typing import Final, List, Set, Dict

VALID_FILE_TYPES: Final[Set] = {".cpp", ".h", ".py", ".ts", ".js", ".rs"}
IGNORE: Final[Set] = {"build", "node_modules", ".git"}
REGEX_MAP: Final[Dict] = {
    ".cpp": re.compile(r'#include\s*["<]([^">]+)[">]'),
    ".h": re.compile(r'#include\s*["<]([^">]+)[">]')
}

class RepoParser:
    """
    Docstring for Parser
    """
    def __init__(self, repo_path: str):
        self.current_id = 0
        self.repo_path = repo_path
        self.file_map = {}
        self.source_files = []
        self.dependency_map = {}
    
    def walk(self) -> List:
        """
        Docstring for walk
        """
        print(f"Walking {self.repo_path}...")
        for root, dirs, files in os.walk(self.repo_path):
            print("Root, dirs, files", root, dirs, files)
            dirs[:] = [d for d in dirs if d not in IGNORE]

            for file in files:
                ext = os.path.splitext(file)[1]
                if ext in VALID_FILE_TYPES:
                    full_path = os.path.join(root, file)
                    relative_path = os.path.relpath(full_path, self.repo_path)
                    if relative_path not in self.file_map:
                        self.file_map[relative_path] = self.current_id
                        self.current_id += 1
                    self.source_files.append(relative_path)
        
        return self.source_files
    
    def scan(self):
        """
        Docstring for scan
        
        :param self: Description
        """
        for relative_file in self.source_files:
            full_path = os.path.join(self.repo_path, relative_file)

            ext = os.path.splitext(relative_file)[1]
            if ext not in REGEX_MAP:
                continue

            try:
                with open(full_path, 'r', encoding="utf-8", errors="ignore") as f:
                    data = f.read()
            except FileNotFoundError:
                print(f"File {relative_file} not found")
                continue
            
            dependencies = re.findall(REGEX_MAP[ext], data)
            if dependencies:
                self.dependency_map[relative_file] = dependencies
    
    def serialize(self):
        """
        Docstring for serialize
        
        :param self: Description
        """
        nodes = []
        edges = []
        for name, id in self.file_map.items():
            nodes.append({
                "id": id,
                "name": name
            })

        # Build the Edges List
        for source_file, dependencies in self.dependency_map.items():
            if source_file not in self.file_map:
                continue
            
            source_id = self.file_map[source_file]
            source_dir = os.path.dirname(source_file)

            for dep_str in dependencies:
                target_id = None
                potential_path = os.path.normpath(os.path.join(source_dir, dep_str))
                
                # Check if this resolved path is in our map
                if potential_path in self.file_map:
                    target_id = self.file_map[potential_path]
                
                if target_id is None:
                    pass
                if target_id is not None:
                    edges.append({
                        "source": source_id,
                        "target": target_id
                    })

        output_data = {
            "nodes": nodes,
            "edges": edges
        }
        
        with open("graph.json", "w", encoding="utf-8") as f:
            json.dump(output_data, f, indent=4)
        
        print(f"Serialized {len(nodes)} nodes and {len(edges)} edges to graph.json")
