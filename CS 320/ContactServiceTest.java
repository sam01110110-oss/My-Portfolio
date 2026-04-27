import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class ContactServiceTest {

    @Test
    void testAddContact() {
        ContactService service = new ContactService();
        Contact c = new Contact("1", "John", "Doe", "1234567890", "123 Street");

        service.addContact(c);
        assertEquals("John", service.getContact("1").getFirstName());
    }

    @Test
    void testAddDuplicateContact() {
        ContactService service = new ContactService();
        Contact c = new Contact("1", "John", "Doe", "1234567890", "123 Street");

        service.addContact(c);

        assertThrows(IllegalArgumentException.class, () -> service.addContact(c));
    }

    @Test
    void testAddNullContact() {
        ContactService service = new ContactService();

        assertThrows(IllegalArgumentException.class, () -> service.addContact(null));
    }

    @Test
    void testDeleteContact() {
        ContactService service = new ContactService();
        Contact c = new Contact("1", "John", "Doe", "1234567890", "123 Street");

        service.addContact(c);
        service.deleteContact("1");

        assertThrows(IllegalArgumentException.class, () -> service.getContact("1"));
    }

    @Test
    void testDeleteNonexistentContact() {
        ContactService service = new ContactService();

        assertThrows(IllegalArgumentException.class, () -> service.deleteContact("1"));
    }

    @Test
    void testUpdateContact() {
        ContactService service = new ContactService();
        Contact c = new Contact("1", "John", "Doe", "1234567890", "123 Street");

        service.addContact(c);

        service.updateFirstName("1", "Mike");
        service.updateLastName("1", "Smith");
        service.updatePhone("1", "0987654321");
        service.updateAddress("1", "456 Ave");

        Contact updated = service.getContact("1");

        assertEquals("Mike", updated.getFirstName());
        assertEquals("Smith", updated.getLastName());
    }

    @Test
    void testUpdateInvalidContact() {
        ContactService service = new ContactService();

        assertThrows(IllegalArgumentException.class, () ->
                service.updateFirstName("999", "Test"));
    }

    @Test
    void testGetNonexistentContact() {
        ContactService service = new ContactService();

        assertThrows(IllegalArgumentException.class, () ->
                service.getContact("999"));
    }
}