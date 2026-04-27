/**
 * The Contact class represents a single contact in the application.
 * Each contact has a unique ID, first name, last name, phone number, and address.
 * The class ensures that all fields meet the required validation rules.
 */
public class Contact {

    // Unique identifier for the contact; cannot be changed once set
    private final String contactId;

    // Contact's first name; must not be null or longer than 10 characters
    private String firstName;

    // Contact's last name; must not be null or longer than 10 characters
    private String lastName;

    // Contact's phone number; must be exactly 10 digits
    private String phone;

    // Contact's address; must not be null or longer than 30 characters
    private String address;

    /**
     * Constructor for creating a new Contact object.
     * Validates all input fields according to the requirements.
     *
     * @param contactId Unique contact ID (max 10 chars, not null)
     * @param firstName Contact's first name (max 10 chars, not null)
     * @param lastName Contact's last name (max 10 chars, not null)
     * @param phone Contact's phone number (exactly 10 digits, not null)
     * @param address Contact's address (max 30 chars, not null)
     * @throws IllegalArgumentException if any field is invalid
     */
    public Contact(String contactId, String firstName, String lastName, String phone, String address) {
        if (contactId == null || contactId.length() > 10) {
            throw new IllegalArgumentException("Invalid contact ID");
        }
        if (firstName == null || firstName.length() > 10) {
            throw new IllegalArgumentException("Invalid first name");
        }
        if (lastName == null || lastName.length() > 10) {
            throw new IllegalArgumentException("Invalid last name");
        }
        if (phone == null || !phone.matches("\\d{10}")) {
            throw new IllegalArgumentException("Invalid phone number");
        }
        if (address == null || address.length() > 30) {
            throw new IllegalArgumentException("Invalid address");
        }

        this.contactId = contactId;
        this.firstName = firstName;
        this.lastName = lastName;
        this.phone = phone;
        this.address = address;
    }

    /**
     * Updates the first name of the contact.
     * @param firstName New first name (max 10 chars, not null)
     * @throws IllegalArgumentException if invalid
     */
    public void setFirstName(String firstName) {
        if (firstName == null || firstName.length() > 10) {
            throw new IllegalArgumentException("Invalid first name");
        }
        this.firstName = firstName;
    }

    /**
     * Updates the last name of the contact.
     * @param lastName New last name (max 10 chars, not null)
     * @throws IllegalArgumentException if invalid
     */
    public void setLastName(String lastName) {
        if (lastName == null || lastName.length() > 10) {
            throw new IllegalArgumentException("Invalid last name");
        }
        this.lastName = lastName;
    }

    /**
     * Updates the phone number of the contact.
     * @param phone New phone number (exactly 10 digits, not null)
     * @throws IllegalArgumentException if invalid
     */
    public void setPhone(String phone) {
        if (phone == null || !phone.matches("\\d{10}")) {
            throw new IllegalArgumentException("Invalid phone number");
        }
        this.phone = phone;
    }

    /**
     * Updates the address of the contact.
     * @param address New address (max 30 chars, not null)
     * @throws IllegalArgumentException if invalid
     */
    public void setAddress(String address) {
        if (address == null || address.length() > 30) {
            throw new IllegalArgumentException("Invalid address");
        }
        this.address = address;
    }

    /**
     * Returns the unique contact ID.
     * @return contact ID
     */
    public String getContactId() {
        return contactId;
    }

    /**
     * Returns the first name of the contact.
     * @return first name
     */
    public String getFirstName() {
        return firstName;
    }

    /**
     * Returns the last name of the contact.
     * @return last name
     */
    public String getLastName() {
        return lastName;
    }

    /**
     * Returns the phone number of the contact.
     * @return phone number
     */
    public String getPhone() {
        return phone;
    }

    /**
     * Returns the address of the contact.
     * @return address
     */
    public String getAddress() {
        return address;
    }
}