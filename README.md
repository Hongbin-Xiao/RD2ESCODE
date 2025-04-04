# RD2ESCODE: Automated Transformation of Requirements Documents into Embedded System Code

![License](https://img.shields.io/badge/license-MIT-blue)
![Version](https://img.shields.io/badge/version-1.0.0-green)
![Python](https://img.shields.io/badge/python-3.7+-orange)

## 📝 Overview

RD2ESCODE is an innovative framework designed to automatically transform embedded system requirements documents into executable code. This project addresses the time-consuming and error-prone nature of requirements validation in embedded system development by leveraging Large Language Models (LLMs) to rapidly generate verifiable embedded code from requirements documentation.

### Key Features

- 🔄 **Automated Transformation Process**: Direct generation of embedded system code from requirements documents
- 🧠 **Prompt-based Fine-tuning Method**: Enables LLMs to understand the complex relationship between embedded code and requirements documentation
- 👨‍👩‍👧‍👦 **Collaborative Code Generation Framework**: Utilizes multiple models to enhance code quality
- 🛠️ **Multiple Programming Style Support**: Generates code in structured, modular, and object-oriented programming styles
- ✅ **Automatic Error Correction**: Guides code corrections through a teacher model to improve code quality

## 🚀 Performance Improvements

Compared to existing methods, RD2ESCODE achieves significant improvements across multiple metrics:

- **Pass@1**: Approximately 57% improvement over directly using LLMs and 50% over the SCOT method
- **Average Test Case Pass Rate**: 44% improvement
- **Compilation Pass Rate**: Substantial enhancement

## 🏗️ Framework Architecture

The RD2ESCODE framework consists of four main stages:

1. **Reference Input Generation**: Producing high-quality reference code and corresponding requirements documents
2. **Code Generation Model Building**: Generating code based on fundamental prompts and employing compiler feedback methods
3. **Teacher Model for Code Correction**: Analyzing the relationship between code and requirements to provide correction strategies
4. **Code Enhancement**: Improving generated code based on the teacher model's correction strategies

![Overview_00](https://github.com/user-attachments/assets/e5b967b3-12fb-4d54-8669-4eb7dce9cb06)
![prompts_00(1)](https://github.com/user-attachments/assets/15969c5e-e000-489b-9e33-b17dbba59221)
![Reference example_00](https://github.com/user-attachments/assets/5db547b3-6e3d-401d-9b50-d3ee8a76413f)



## 📊 Dataset
We created a dataset comprising 15 simulated implementations of embedded systems to evaluate our method. The dataset includes:

- 15 requirements documents for embedded systems ranging from simple to complex
- Reference code in three different programming styles
- Executable simulation systems implemented in Proteus 8 Professional
