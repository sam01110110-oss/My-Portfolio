Corner Grocer Item Tracking Program

Author: Samuel Harrison Parnell
Course: CS-210: Programming Languages
Language: C++

Project Summary

This project, the Corner Grocer Item Tracking Program, was developed to help a small grocery store efficiently track and analyze the frequency of purchased items. The program reads item data from a text file, counts the occurrences of each item, and provides multiple user options, such as displaying item frequencies, searching for specific items, and visualizing data as a histogram. The program solves the problem of manual data tracking by automating analysis, ensuring accuracy, and saving time.

I designed clean, modular, and readable C++ code using object-oriented principles. The ItemTracker class structure allowed for well-organized functionality, making it easy to maintain and extend. I also implemented file handling and data validation effectively, ensuring the program could handle user errors gracefully. Additionally, I made sure the output was user-friendly and easy to interpret.

I could improve efficiency by implementing unordered maps (std::unordered_map) instead of ordered maps (std::map) for faster lookups. I could also add exception handling for more robust file operations and expand the histogram visualization to include color or alignment formatting for better readability. These improvements would make the code more efficient, secure, and user-friendly.

The most challenging part was designing the data flow between file input, map processing, and user interaction. Initially, I had trouble keeping the logic organized as new features were added. I overcame this by breaking the project into smaller methods, testing each one independently, and using debugging tools in my IDE to trace data step-by-step. I also referenced C++ documentation and Stack Overflow for syntax clarification and best practices.

This project strengthened my understanding of C++ syntax, file handling, loops, conditionals, and data structures like maps. These skills are transferable to future software development work in other languages, especially those that rely on similar object-oriented and data-processing concepts, such as Java and Python. It also improved my ability to write readable, maintainable code and handle user input effectively.

I made the program maintainable by keeping my functions concise, using descriptive variable names, and adding meaningful comments throughout the code. I separated core logic from user interface code to make future updates easier. By adhering to consistent naming conventions and indentation, the program is easily readable and adaptable for future enhancements, such as adding new data analysis features or a graphical interface.