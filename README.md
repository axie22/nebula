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

### Building for WebAssembly

This project uses **Emscripten** to build a WebAssembly module. You must define your `ENDPOINT_URL` environment variable at compile-time since WebAssembly cannot dynamically read `.env` files.

1. Create a `build` directory:

   ```bash
   mkdir build && cd build
   ```

2. Compile the project with Emscripten, specifying your backend endpoint:
   ```bash
   emcmake cmake .. -DENDPOINT_URL="http://localhost:8000"
   make
   ```

### Running Locally

Because of strict browser CORS security policies, you cannot open the generated `CodeNebula.html` via the `file://` protocol. You must serve the files via a local HTTP server.

1. Ensure you are in the `build` directory:

   ```bash
   cd build
   ```

2. Start a local HTTP server:

   ```bash
   python3 -m http.server 8080
   ```

3. Open your browser and navigate to:
   [http://localhost:8080/CodeNebula.html](http://localhost:8080/CodeNebula.html)

### Running Native (Non-Web)

If you are compiling for a native desktop platform (not WebAssembly), you can source your `.env` file prior to running:

```bash
set -a; source ../.env; set +a
./CodeNebula
```
