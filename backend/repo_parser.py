import os
from typing import Final, List, Set

VALID_FILE_TYPES: Final[Set] = {".cpp", ".h", ".py", ".ts", ".js", ".rs"}
IGNORE: Final[Set] = {"build/", "node_modules", ".git"}

class RepoParser:
    """
    Docstring for Parser
    """
    def __init__(self, repo_path: str):
        self.current_id = 0
        self.repo_path = repo_path
        self.file_map = {}
        self.source_files = []
    
    def walk(self) -> List:
        """
        Docstring for walk
        """
        print(f"Walking {self.repo_path}...")
        for root, dirs, files in os.walk(self.repo_path):
            print("Root, dirs, files", root, dirs, files)
            dirs = [d for d in dirs if d not in IGNORE]

            for file in files:
                ext = os.path.splitext(file)[1]
                if ext in VALID_FILE_TYPES:
                    full_path = os.path.join(root, file)
                    if full_path not in self.file_map:
                        self.file_map[full_path] = self.current_id
                        self.current_id += 1
                    self.source_files.append(full_path)
        
        return self.source_files