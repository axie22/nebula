import re
import tempfile
import subprocess
from contextlib import contextmanager

class GitCloner:
    def __init__(self, github_url: str) -> None:
        self.github_url = github_url
        self.url_pattern = re.compile(r"^https://github\.com/[a-zA-Z0-9-]+/[a-zA-Z0-9_.-]+(?:\.git)?$")
    
    @contextmanager
    def clone(self):
        """
        Clones github repo and yeilds the cloned directory
        
        :param self: Description
        """
        if not self.url_pattern.match(self.github_url):
            raise ValueError("Invalid GitHub URL")

        with tempfile.TemporaryDirectory() as temp_dir:
            try:
                subprocess.run(
                    ['git', 'clone', '--depth', '1', self.github_url, '.'], 
                    cwd=temp_dir, 
                    check=True,
                    capture_output=True
                )

                # hand over path to caller
                yield temp_dir
                
            except subprocess.CalledProcessError as e:
                raise RuntimeError(f'Git clone failed: {e.stderr.decode()}') from e    