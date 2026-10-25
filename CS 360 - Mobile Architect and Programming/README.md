# WeightTracker Mobile Application

## Project Overview

WeightTracker is an Android mobile application developed in Java using Android Studio. The purpose of the application is to help users track their weight, manage weight goals, and monitor their progress over time. The app provides a simple way for users to record measurements, review their weight history, and work toward their personal goals.

## Requirements and User Needs

The main requirements for the application were to provide account creation and login, store weight entries, display weight history, and allow users to set weight goals. The application also includes an SMS notification feature intended to notify users when they reach their goals. These features address the need for a convenient way to track personal progress while keeping each user's information separate.

## User Interface Design

The application includes a login and account creation screen, a weight history screen, an add weight screen, and a notifications screen. The login screen allows users to access their accounts, while the weight history screen displays saved measurements and current goal information. The add weight screen allows users to enter a date, weight, goal weight, and phone number. The notifications screen allows users to manage SMS permissions and view their notification status.

I designed the interface around the tasks users would need to complete most often. Clear labels, buttons, and a consistent layout help users navigate the application without unnecessary steps. I used a dark interface with green accents to give the application a consistent appearance. These design choices help make the main features easier to locate and understand, supporting a user-centered experience.

## Coding Approach and Development Strategies

I developed the application in Java using Android Studio. I separated the application's responsibilities across different activities and used a database helper class to manage account information and weight entries. SQLite provides local storage, allowing the application to save information between sessions. I also organized the code into separate classes to make it easier to understand, test, and maintain.

Working on the application in smaller steps helped me identify problems without having to troubleshoot the entire project at once. I focused on implementing individual features and checking that they worked before moving on to other parts of the application. In future projects, I can apply this approach by planning the interface, organizing the code, developing features incrementally, and testing throughout the development process.

## Testing and Functionality

I tested the application using an Android emulator. I checked account creation and login, verified that different accounts maintained separate information, and tested adding weight entries and displaying them in the weight history. I also tested navigation between screens and the SMS permission and notification functionality.

Testing was important because it helped me identify problems that were not immediately obvious during development. For example, I encountered issues involving SMS permissions and session information that required additional troubleshooting. Resolving these problems reinforced the importance of testing both normal application behavior and features that depend on Android permissions or stored data. Testing also helped me verify that the different parts of the application worked together as intended.

## Challenges and Innovation

One of my main challenges was getting the SMS notification feature to work correctly with Android's permission system and the application's account session. I had to investigate the problem, check the emulator's permission settings, and make adjustments until the application could report a successful SMS operation. This experience taught me that developing a mobile application involves more than creating screens and writing code. It also requires understanding how the application interacts with the operating system and how different components work together.

Another challenge was connecting the user interface to the database. I needed to ensure that information entered by users was saved correctly and displayed in the appropriate account's history. By working through these issues, I gained a better understanding of how the interface, database, and application logic work together to create a functional mobile application.

## Demonstrated Skills

I was particularly successful in implementing the database functionality and connecting it to the application's user interface. The application allows users to create accounts, log in, save weight measurements, and view their recorded information. Implementing these features required me to work with Java, SQLite, Android activities, and data handling.

This project strengthened my understanding of how mobile applications combine user-centered design, programming, data persistence, and testing. It also gave me experience troubleshooting development problems and improving an application through repeated testing. These skills will be useful in future software development projects.

## Technologies Used

Java

Android Studio

SQLite

Android SDK

XML layouts

Android Emulator

Android SMS permissions and notification functionality
