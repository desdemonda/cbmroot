#include <iostream>
#include "Message.hpp"

uint16_t buf[8] = {
    0x8012, /* start of message */
    0x9666,
    0xA008,
    0x0403,
    0x0100,
    0x5030,
    0x0E00,
    0xB1D0, /* end of message */
};

void read()
{
    spadic::MessageReader r;

    r.add_buffer(buf, 8);

    while(auto m = r.get_message()) {
        if (m->is_hit()) {
            auto& s = m->samples();
            std::cout << "got " << s.size() << " samples" << std::endl;
        }
    }
}

int main()
{
    try {
        read();
    }
    catch (spadic::MessageError& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
