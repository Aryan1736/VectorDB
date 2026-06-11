# 🚀 VectorDB

> A high-performance **Vector Database built completely from scratch in C++**, featuring multiple nearest-neighbor search algorithms, semantic document retrieval, Retrieval-Augmented Generation (RAG), Ollama integration, and an interactive web dashboard.

Inspired by modern vector databases like **Pinecone**, **Weaviate**, **Chroma**, and **Milvus**, this project demonstrates how vector search systems work internally—from indexing vectors to powering AI applications.

---

# ✨ Features

## 🔍 Vector Search Engine

Implemented multiple search algorithms from scratch:

- ⚡ Brute Force Search
- 🌳 KD-Tree
- 🕸️ HNSW (Hierarchical Navigable Small World)

Compare their speed, accuracy, and scalability in real time.

---

## 📏 Distance Metrics

Supports multiple similarity functions:

- 🎯 Cosine Similarity
- 📐 Euclidean Distance
- 📊 Manhattan Distance

---

## 🗄️ Vector Database Operations

- ➕ Insert vectors
- ❌ Delete vectors
- 🔎 K-Nearest Neighbor Search
- 📈 Benchmark search algorithms
- 🕸️ HNSW graph statistics

---

## 📄 Document Database

Store and search real text documents.

Features:

- ✂️ Automatic document chunking
- 🧠 Semantic embeddings
- 📚 Document retrieval
- ⚡ HNSW indexing for fast search

---

## 🤖 Retrieval-Augmented Generation (RAG)

Ask questions directly over your stored documents.

Pipeline:

```text
Question
   ↓
Embedding
   ↓
HNSW Retrieval
   ↓
Relevant Chunks
   ↓
LLM Generation
   ↓
Answer
```

---

## 🦙 Ollama Integration

Runs completely locally.

### Embedding Model

```text
nomic-embed-text
```

### Language Model

```text
llama3.2
```

No OpenAI API required.

---

## 🌐 REST API

Includes endpoints for:

- Vector Search
- Vector Insertion & Deletion
- Benchmarking
- HNSW Statistics
- Document Management
- RAG Question Answering
- Ollama Status Monitoring

---

## 🎨 Interactive Dashboard

Includes:

- 🔍 Search Panel
- 📊 Benchmark Dashboard
- 📄 Document Manager
- 🤖 AI Chat Interface
- 📈 PCA Visualization
- 🕸️ HNSW Statistics

---

# 🏗️ Architecture

```text
                    ┌─────────────────┐
                    │ User Query      │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Ollama Embedder │
                    └────────┬────────┘
                             │
                             ▼
                  ┌──────────────────────┐
                  │ Vector Database      │
                  ├──────────────────────┤
                  │ Brute Force          │
                  │ KD Tree              │
                  │ HNSW                 │
                  └────────┬─────────────┘
                           │
                           ▼
                ┌────────────────────┐
                │ Retrieved Context  │
                └────────┬───────────┘
                         │
                         ▼
                ┌────────────────────┐
                │ Ollama (LLM)       │
                └────────┬───────────┘
                         │
                         ▼
                ┌────────────────────┐
                │ Generated Answer   │
                └────────────────────┘
```

---

# 🧠 Algorithms Implemented

## ⚡ Brute Force Search

Checks every vector.

### Complexity

```text
Insert : O(1)
Search : O(N × D)
```

---

## 🌳 KD-Tree

Space partitioning tree for exact nearest-neighbor search.

### Complexity

```text
Average Search : O(log N)
Worst Case     : O(N)
```

---

## 🕸️ HNSW

Hierarchical Navigable Small World Graph.

Used in:

- Pinecone
- Weaviate
- Chroma
- Milvus

### Complexity

```text
Insert : O(log N)
Search : O(log N)
```

---

# 📂 Project Structure

```text
VectorDB
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

# ⚙️ Setup

## 📦 Requirements

- C++17
- g++
- Ollama
- Windows / Linux

---

## 🦙 Install Ollama Models

```bash
ollama pull nomic-embed-text
ollama pull llama3.2
```

---

## 🔨 Build

```bash
g++ main.cpp src/*.cpp -Iinclude -O2 -lws2_32 -o db
```

---

## ▶️ Run

```bash
./db
```

Server starts at:

```text
http://localhost:8080
```

---

# 📊 Example Benchmark

```text
Dataset Size : 10,000 vectors

Brute Force : 3775 μs
KD-Tree     :   35 μs
HNSW        :  962 μs
```

---

# 🎯 Learning Outcomes

This project demonstrates:

- Vector Similarity Search
- Approximate Nearest Neighbor Search
- Graph-Based Indexing
- Semantic Search
- Retrieval-Augmented Generation
- REST API Development
- Local LLM Integration
- C++ System Design
- Modern Vector Database Internals

---

# 🚀 Future Improvements

- 💾 Persistent Storage
- 📂 Multiple Collections
- 🏷️ Metadata Filtering
- 🔀 Hybrid Search (BM25 + Vector)
- 📄 PDF Ingestion
- ⚡ Streaming LLM Responses
- 🕸️ Interactive HNSW Graph Viewer
- 👥 Authentication & Multi-user Support

---

# ⭐ Why This Project?

Most developers use vector databases through APIs.

This project builds one from scratch.

It demonstrates the core concepts behind modern AI infrastructure including:

- Semantic Search
- ANN Algorithms
- Vector Indexing
- RAG Systems
- Local AI Deployment
- Search Engine Architecture
