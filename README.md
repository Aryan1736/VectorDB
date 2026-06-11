# VectorDB

A Vector Database built from scratch in C++ featuring multiple nearest-neighbor search algorithms, real document embeddings, Retrieval-Augmented Generation (RAG), and a REST API.

This project was built to understand how modern vector databases such as Pinecone, Weaviate, Chroma, and Milvus work internally.

---

## Features

### Vector Search Algorithms

- Brute Force K-NN Search
- KD-Tree Search
- HNSW (Hierarchical Navigable Small World)
- Benchmarking across all algorithms

### Distance Metrics

- Cosine Similarity
- Euclidean Distance
- Manhattan Distance

### Vector Database

- Insert vectors
- Delete vectors
- K-Nearest Neighbor Search
- Benchmark search performance
- HNSW graph statistics

### Document Database

- Store embedded documents
- Automatic text chunking
- Semantic document retrieval
- HNSW indexing for embeddings

### RAG Pipeline

- Embed documents using Ollama
- Retrieve relevant document chunks
- Generate answers using a local LLM
- Fully local AI workflow

### REST API

- Search vectors
- Insert/Delete vectors
- Benchmark algorithms
- Manage documents
- Ask questions over stored documents

### Frontend Dashboard

- Search interface
- Benchmark panel
- Document management
- AI chat interface
- HNSW visualization support

---

# Architecture

```text
User Query
     |
     v
Embedding Model (Ollama)
     |
     v
Vector Database
     |
     +---- Brute Force
     |
     +---- KD Tree
     |
     +---- HNSW
     |
     v
Nearest Neighbors
     |
     v
Retrieved Context
     |
     v
LLM (llama3.2)
     |
     v
Generated Answer
```

---

# Project Structure

```text
VectorDB/
│
├── include/
│   ├── VectorDB.hpp
│   ├── DocumentDB.hpp
│   ├── HNSW.hpp
│   ├── KDTree.hpp
│   ├── BruteForce.hpp
│   ├── Distance.hpp
│   ├── Chunker.hpp
│   ├── OllamaClient.hpp
│   ├── RAGEngine.hpp
│   └── Server.hpp
│
├── src/
│   ├── VectorDB.cpp
│   ├── DocumentDB.cpp
│   ├── HNSW.cpp
│   ├── KDTree.cpp
│   ├── BruteForce.cpp
│   ├── Chunker.cpp
│   ├── OllamaClient.cpp
│   ├── RAGEngine.cpp
│   ├── Server.cpp
│   └── DemoData.cpp
│
├── index.html
├── main.cpp
└── README.md
```

---

# Algorithms Implemented

## Brute Force

Computes distance from the query to every vector.

### Complexity

```text
Insert : O(1)
Search : O(N × D)
```

Where:

- N = number of vectors
- D = dimensions

Used as the baseline for benchmarking.

---

## KD Tree

Binary space partitioning structure.

### Complexity

```text
Average Search : O(log N)
Worst Case     : O(N)
```

Works well for low-dimensional data.

---

## HNSW

Hierarchical Navigable Small World Graph.

Used in production systems such as:

- Pinecone
- Weaviate
- Chroma
- Milvus

### Complexity

```text
Insert : O(log N)
Search : O(log N)
```

Provides fast approximate nearest-neighbor search.

---

# Ollama Integration

The project uses:

### Embedding Model

```text
nomic-embed-text
```

Used to convert text into dense vectors.

### Language Model

```text
llama3.2
```

Used for answer generation.

---

# REST API

## Status

### GET

```http
/status
```

Returns Ollama status and model information.

---

## Vector Endpoints

### GET

```http
/items
```

List all vectors.

### GET

```http
/search
```

Search vectors.

Parameters:

```text
v
k
metric
algo
```

Example:

```http
/search?v=0.9,0.8,...&k=5&metric=cosine&algo=hnsw
```

---

### GET

```http
/benchmark
```

Compare Brute Force, KD-Tree, and HNSW.

---

### GET

```http
/hnsw-info
```

Returns HNSW layer statistics.

---

### POST

```http
/insert
```

Insert a vector.

---

### DELETE

```http
/delete/{id}
```

Delete a vector.

---

## Document Endpoints

### POST

```http
/doc/insert
```

Insert a document.

Request:

```json
{
  "title": "OS Notes",
  "text": "Deadlock occurs when..."
}
```

---

### GET

```http
/doc/list
```

List stored documents.

---

### DELETE

```http
/doc/delete/{id}
```

Delete a document.

---

### POST

```http
/doc/ask
```

Ask a question using RAG.

Request:

```json
{
  "question": "What is deadlock?"
}
```

Response:

```json
{
  "answer": "Deadlock is a situation..."
}
```

---

# Building

## Requirements

- C++17
- g++
- Ollama

### Install Ollama Models

```bash
ollama pull nomic-embed-text
ollama pull llama3.2
```

---

## Compile

Windows (MSYS2)

```bash
g++ main.cpp src/*.cpp -Iinclude -O2 -lws2_32 -o db
```

---

## Run

```bash
./db
```

Server:

```text
http://localhost:8080
```

---

# Example RAG Flow

```text
Document
   |
   v
Chunker
   |
   v
Ollama Embedding
   |
   v
DocumentDB (HNSW)
   |
   v
Relevant Chunks
   |
   v
LLM (llama3.2)
   |
   v
Answer
```

---

# Benchmarks

Example results on 10,000 vectors:

```text
Brute Force : 3775 us
KD Tree     : 35 us
HNSW        : 962 us
```

Actual results vary based on dataset size and dimensions.

---

# Learning Goals

This project demonstrates:

- Vector similarity search
- Approximate nearest neighbor search
- Graph-based indexing
- Semantic search
- Retrieval-Augmented Generation
- REST API development in C++
- Local LLM integration
- System design of modern vector databases

---

# Future Improvements

- Persistent storage
- Multiple collections
- Metadata filtering
- Hybrid search (BM25 + Vector Search)
- PDF ingestion
- Streaming responses
- Better HNSW visualization
- Authentication & user accounts
