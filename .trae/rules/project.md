# Project Overview
This project is a simple pokemon battle simulator suite. It is written in Rust for maximum performance and safety. The primary purpose of this project is to provide a simple and efficient way to simulate pokemon battles for multiple purposes, including gaming and machine learning. As such, the library should be designed to be as versatile and flexible as possible, while still maintaining a high level of performance and safety.

# Rules
1. The simulator should only account for single-battle scenarios. Multi-battle scenarios, such as team battles or double battles, are out of scope for this project.
2. Use only the data within ./static/csv as the source of truth for pokemon stats, moves, and types.

# Project Structure
This project consists of 3 main components, implemented as seperate projects with their own repositories:
1. The pokemon battle simulator library.
2. A web interface for the simulator, complete with graphics and interactive elements.
3. A python interface for the simulator, complete with a simple and intuitive API. The python interface should be designed to be as easy to use as possible, with a focus on readability and simplicity. The API should provide a way for users to create and simulate pokemon battles, massively in parellel with large batches, as well as access the results of those simulations for RL agent training.
