# Project Two Reflection
## How do you write programs that are maintainable, readable, and adaptable?

Writing maintainable, readable, and adaptable programs starts with organizing code into logical components and giving each component a specific purpose. During Project Two, I used the CRUD Python module that I developed in Project One to handle communication between the dashboard and the MongoDB database. Instead of placing all of the database connection and query code directly into the dashboard, I was able to keep those operations inside the AnimalShelter class. This made the dashboard code easier to understand and allowed the database functionality to be reused without having to rewrite it.

One major advantage of this approach was that it separated the database operations from the user interface. The dashboard could focus on displaying information and responding to user input, while the CRUD module handled creating and retrieving records from MongoDB. This separation also made the project easier to modify and troubleshoot. For example, if the database connection or query needed to be changed, I could make that change in the CRUD module without having to redesign the entire dashboard.

The CRUD Python module could also be useful in future projects that require access to the same type of animal shelter database. I could expand the module to support additional CRUD operations, such as updating and deleting records, or reuse the same structure with a different MongoDB collection or database. This experience showed me the value of creating reusable components rather than writing code specifically for only one application.

## How do you approach a problem as a computer scientist?

I approach a problem as a computer scientist by first identifying the requirements, breaking the problem into smaller parts, and determining what information and tools are needed to solve it. For the Grazioso Salvare project, I first had to understand what information the organization needed from its animal shelter database. I then considered how that information could be queried and presented through dashboard components such as the data table, map, and charts.

My approach to this project differed from some previous assignments because I had to consider multiple parts of a larger system at the same time. Instead of simply writing a program that produced a specific output, I had to connect a database, Python code, and an interactive dashboard to meet a client's requirements. I also had to think about how the user would interact with the dashboard and whether the displayed information was useful for the organization's needs.

In the future, I would use a similar process when creating databases for other clients. I would begin by identifying the client's goals and data requirements, examine the available data, determine the appropriate database structure, and develop queries based on the information the client needs. I would also test the database and application throughout development rather than waiting until the end. Breaking a larger problem into smaller, testable components makes it easier to identify errors and adapt the solution when requirements change.

## What do computer scientists do, and why does it matter?

Computer scientists solve problems by using computing technologies, programming, data, and logical problem-solving techniques to create useful solutions. Their work can help organizations manage information, automate processes, analyze data, and make better decisions. The Grazioso Salvare project is an example of how these skills can be applied to a real-world organization.

For a company such as Grazioso Salvare, having an interactive dashboard makes it easier to work with a large amount of animal shelter data. Instead of manually searching through database records, users can filter the available animals and quickly view information in tables, charts, and maps. This can help employees identify animals that meet specific criteria and make the organization's workflow more efficient.

This project also demonstrated that computer science is not only about writing code. A successful solution has to address the needs of the people who will actually use it. By combining database management, Python programming, and dashboard development, I was able to create a tool that turns raw data into information that can support an organization's decisions. That is one of the reasons computer science matters: it provides practical ways to solve problems and improve how organizations work with information.
