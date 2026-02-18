import os
import re
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
  
