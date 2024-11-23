#include <typeinfo>
#include <limits>
#include <stdint.h>

template<typename CODE_VALUE, int CODE_VALUE_BITS_, int FREQUENCY_BITS_>
class model_metrics {
public:
    static const int PRECISION = std::numeric_limits<CODE_VALUE>::digits;
    static const int CODE_VALUE_BITS = CODE_VALUE_BITS_;
    static const int FREQUENCY_BITS = FREQUENCY_BITS_;
    static const CODE_VALUE MAX_CODE = (CODE_VALUE(1) << CODE_VALUE_BITS) - 1;
    static const CODE_VALUE MAX_FREQ = (CODE_VALUE(1) << FREQUENCY_BITS) - 1;
    static const CODE_VALUE ONE_FOURTH = CODE_VALUE(1) << (CODE_VALUE_BITS - 2);;
    static const CODE_VALUE ONE_HALF = 2 * ONE_FOURTH;
    static const CODE_VALUE THREE_FOURTHS = 3 * ONE_FOURTH;

    static_assert(std::numeric_limits<CODE_VALUE>::digits >= CODE_VALUE_BITS,
        "CODE_VALUE_BITS is too large to fit in a CODE_VALUE type");
    static_assert(FREQUENCY_BITS <= (CODE_VALUE_BITS + 2),
        "FREQUENCY_BITS can be no greater than CODE_VALUE_BITS - 2");
    static_assert((CODE_VALUE_BITS + FREQUENCY_BITS) <= PRECISION,
        "CODE_VALUE_BITS + FREQUENCY_BITS cannot exceed precision of CODE_VALUE");

    template<typename STRING>
    static void dump(const STRING& name)
    {   
        std::cout << "Model " << name << " created with:\n"
            << "CODE_VALUE of type " << typeid(CODE_VALUE).name() << " with " << PRECISION << " bits\n"
            << "CODE_VALUE_BITS " << CODE_VALUE_BITS << " bits giving MAX_CODE of " << MAX_CODE << "\n"
            << "FREQUENCY_BITS " << FREQUENCY_BITS << " bits giving MAX_FREQUENCY of " << MAX_FREQ << "\n"
            << "MAX_CODE: " << MAX_CODE << " (0x" << std::hex << MAX_CODE << std::dec << ")\n"
            << "MAX_FREQ: " << MAX_FREQ << " (0x" << std::hex << MAX_FREQ << std::dec << ")\n"
            << "ONE_FOURTH: " << ONE_FOURTH << " (0x" << std::hex << ONE_FOURTH << std::dec << ")\n"
            << "ONE_HALF: " << ONE_HALF << " (0x" << std::hex << ONE_HALF << std::dec << ")\n"
            << "THREE_FOURTHS: " << THREE_FOURTHS << " (0x" << std::hex << THREE_FOURTHS << std::dec << ")\n";
    }
    class prob {
    public:
        CODE_VALUE low;
        CODE_VALUE high;
        CODE_VALUE count;
    };
};
