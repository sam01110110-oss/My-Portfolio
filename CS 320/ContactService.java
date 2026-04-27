import java.util.HashMap;
import java.util.Map;

/**
 * The ContactService class manages a collection of Contact objects in-memory.
 * It provides methods to add, delete, and update contacts, as well as retrieve a contact by its ID.
 */
public class ContactService {

    // Map to store contacts using contact ID as the key
    private Map<String, Contact> contacts = new HashMap<>();

    /**
     * Retrieves a contact by its unique ID.
     *
     * @param contactId The ID of the contact to retrieve.
     * @return The Contact object with the specified ID.
     * @throws IllegalArgumentException if the contact ID does not exist.
     */
    public Contact getContact(String contactId) {
        if (!contacts.containsKey(contactId)) {
            throw new IllegalArgumentException("Contact not found");
        }
        return contacts.get(contactId);
    }

    /**
     * Adds a new contact to the service.
     *
     * @param contact The Contact object to add.
     * @throws IllegalArgumentException if the contact is null or the contact ID already exists.
     */
    public void addContact(Contact contact) {
        if (contact == null || contacts.containsKey(contact.getContactId())) {
            throw new IllegalArgumentException("Contact already exists or is null");
        }
        contacts.put(contact.getContactId(), contact);
    }

    /**
     * Deletes a contact by its unique ID.
     *
     * @param contactId The ID of the contact to delete.
     * @throws IllegalArgumentException if the contact ID does not exist.
     */
    public void deleteContact(String contactId) {
        if (!contacts.containsKey(contactId)) {
            throw new IllegalArgumentException("Contact does not exist");
        }
        contacts.remove(contactId);
    }

    /**
     * Updates the first name of a contact.
     *
     * @param contactId The ID of the contact to update.
     * @param firstName The new first name (must follow Contact validation rules).
     */
    public void updateFirstName(String contactId, String firstName) {
        getContact(contactId).setFirstName(firstName);
    }

    /**
     * Updates the last name of a contact.
     *
     * @param contactId The ID of the contact to update.
     * @param lastName The new last name (must follow Contact validation rules).
     */
    public void updateLastName(String contactId, String lastName) {
        getContact(contactId).setLastName(lastName);
    }

    /**
     * Updates the phone number of a contact.
     *
     * @param contactId The ID of the contact to update.
     * @param phone The new phone number (must follow Contact validation rules).
     */
    public void updatePhone(String contactId, String phone) {
        getContact(contactId).setPhone(phone);
    }

    /**
     * Updates the address of a contact.
     *
     * @param contactId The ID of the contact to update.
     * @param address The new address (must follow Contact validation rules).
     */
    public void updateAddress(String contactId, String address) {
        getContact(contactId).setAddress(address);
    }
}