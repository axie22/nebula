from pathlib import Path
from backend.repo_parser import RepoParser
import json


def main():
    current_dir = Path(__file__).parent
    repo_dir = current_dir / "test_repo"
    parser = RepoParser(str(repo_dir))
    parser.walk()
    print(f"Source files: {parser.source_files}")
    print("Scanning...")
    parser.scan()
    print("Dependency map", parser.dependency_map)
    
    output_path = "graph.json"
    parser.serialize(output_path)


if __name__ == "__main__":
    main()