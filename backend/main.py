from backend.gitcloner import GitCloner
from backend.repo_parser import RepoParser

def process_repo(url: str):
    cloner = GitCloner(url)
    
    with cloner.clone() as repo_path:
        print(f"Repo cloned to temporary path: {repo_path}")
        
        # Instantiate Parser with the temp path
        parser = RepoParser(repo_path)
        
        # Run the pipeline
        parser.walk()
        parser.scan()
        parser.serialize("graph.json")
        
    print("Processing complete. Temp files cleaned up.")

if __name__ == "__main__":
    process_repo("https://github.com/axie22/Orbit")