# Nebula

## Workflow

1. User pastes URL into a text box on your website.
2. Website sends URL to your Python Backend.
3. Backend clones repo, parses it, and returns:

```JSON
{
  "nodes": [
    {"id": 0, "name": "main.cpp", "size": 100},
    {"id": 1, "name": "Simulation.h", "size": 50}
  ],
  "edges": [
    {"source": 0, "target": 1}
  ]
}
```

4. C++ App (WASM) receives this JSON.
5. C++ App runs sim.InitFromJSON(...)

## Usage

**Set Environment Variables**

```bash
set -a; source .env; set +a

./build/CodeNebula
```
