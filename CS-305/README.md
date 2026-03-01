# CS-305: Artemis Financial Practices for Secure Software

## Client Overview
Artemis Financial is a financial services company that handles sensitive financial data. The client needed improvements to their web application's security, specifically to protect data transmitted between clients and the server. The main objective of this project was to identify vulnerabilities, implement secure communication, and ensure that the application remained functional and free of newly introduced security risks.

## Secure Coding & Vulnerability Assessment
During the project, I successfully identified and mitigated potential security risks by analyzing project dependencies and implementing secure communication protocols. Using SSL/TLS encryption and SHA-256 hashing, I ensured that client-server data remained confidential and its integrity verified. Secure coding is critical because it reduces the likelihood of data breaches, protects customer trust, and contributes to the company’s overall stability and reputation.

## Challenges & Lessons Learned
The most challenging part of the project was interpreting the automated vulnerability scan results and determining which vulnerabilities were high risk versus informational. This process was helpful because it taught me how to prioritize remediation efforts based on severity and real-world impact.

## Layers of Security & Future Assessment
To increase layers of security, I updated vulnerable dependencies, enforced HTTPS using TLS, and implemented SHA-256 checksums. In the future, I would continue using tools such as OWASP Dependency-Check, static code analysis, and dynamic testing to assess vulnerabilities and select the most effective mitigation strategies.

## Ensuring Functionality & Security
After refactoring the code, I tested the application to confirm that it compiled correctly, the HTTPS endpoint was accessible, and the SHA-256 checksum was generated accurately. I reran automated vulnerability scans to ensure that no new vulnerabilities were introduced, demonstrating that security improvements did not compromise functionality.

## Resources, Tools, and Coding Practices
The tools and practices I used included:
- OWASP Dependency-Check for dependency scanning
- Maven dependency management
- SHA-256 hashing algorithm
- SSL/TLS configuration
- Manual code review
- Secure coding principles and testing

These tools and practices will be valuable for future software development and security-focused projects.

## Portfolio Value for Future Employers
This project demonstrates my ability to conduct a vulnerability assessment, refactor code securely, implement encryption and secure communication, and validate application functionality. It provides a clear example of my technical skills, knowledge of security best practices, and ability to apply them in a real-world financial application context.
