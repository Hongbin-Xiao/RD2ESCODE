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

![Framework Overview](https://via.placeholder.com/800x400?text=RD2ESCODE+Framework)

## 💻 Quick Start

### Requirements
