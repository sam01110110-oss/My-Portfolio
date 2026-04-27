import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class ContactTest {

    @Test
    void testValidContact() {
        Contact contact = new Contact("123", "John", "Doe", "1234567890", "123 Street");
        assertEquals("John", contact.getFirstName());
        assertEquals("Doe", contact.getLastName());
    }

    @Test
    void testBoundaryValues() {
        String max10 = "1234567890";
        String max30 = "123456789012345678901234567890";

        Contact contact = new Contact("123", max10, max10, "1234567890", max30);

        assertEquals(max10, contact.getFirstName());
        assertEquals(max30, contact.getAddress());
    }

    @Test
    void testInvalidContactId() {
        assertThrows(IllegalArgumentException.class, () ->
                new Contact(null, "John", "Doe", "1234567890", "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("12345678901", "John", "Doe", "1234567890", "123 Street"));
    }

    @Test
    void testInvalidFirstName() {
        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", null, "Doe", "1234567890", "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "", "Doe", "1234567890", "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "LongFirstName", "Doe", "1234567890", "123 Street"));
    }

    @Test
    void testInvalidLastName() {
        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", null, "1234567890", "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "", "1234567890", "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "VeryLongLastName", "1234567890", "123 Street"));
    }

    @Test
    void testInvalidPhone() {
        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "Doe", null, "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "Doe", "123", "123 Street"));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "Doe", "123abc7890", "123 Street"));
    }

    @Test
    void testInvalidAddress() {
        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "Doe", "1234567890", null));

        assertThrows(IllegalArgumentException.class, () ->
                new Contact("1", "John", "Doe", "1234567890",
                        "This address is way too long to be valid"));
    }

    @Test
    void testSettersValid() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Street");

        contact.setFirstName("Mike");
        contact.setLastName("Smith");
        contact.setPhone("0987654321");
        contact.setAddress("456 Ave");

        assertEquals("Mike", contact.getFirstName());
        assertEquals("Smith", contact.getLastName());
    }

    @Test
    void testSettersInvalid() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Street");

        assertThrows(IllegalArgumentException.class, () -> contact.setFirstName(null));
        assertThrows(IllegalArgumentException.class, () -> contact.setPhone("123"));
        assertThrows(IllegalArgumentException.class, () -> contact.setAddress(null));
    }
}