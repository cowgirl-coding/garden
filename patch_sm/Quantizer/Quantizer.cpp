#include <array>

#include "daisy_patch_sm.h"
#include "daisysp.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

/** Hardware object for the patch_sm */
DaisyPatchSM patch;

/* Eurorack uses volt per octave scaling so mapping volts to Hz is
conveniently linear. */
constexpr float semitone = 1.f / 12.f;

int main(void)
{
    // This is the set of all notes in the 12-tone scale. True notes are active
    // and can be quantized to, while false notes are disabled.
    std::array<bool, 12> notes = { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 };

    /** Initialize the patch_sm hardware object */
    patch.Init();

    float led_brightness = 0.f;

    /** Loop forever */
    while(1)
    {
        patch.ProcessAllControls();
        // const bool state = patch.gate_in_1.State();
        // // Only update CV output when a gate signal is received.

        // if (!state) {
        //     //if (led_brightness > 0.f) led_brightness -= 0.5f;
        //     patch.WriteCvOut(CV_OUT_2, 0);
        // }
        // else
        // {
        //     patch.WriteCvOut(CV_OUT_2, 5);

        // }
        //led_brightness = 5.f;

        // What is the allowed range of output for this value? Is it different
        // for different CV inputs?
        const float value = patch.GetAdcValue(CV_5);
        const float octave = floorf(value);
        const float note = (value - octave) / semitone;
        bool found = false;
        int distance = 1;
        const int index = round(note);
        // If the note is on the high end, we start by looking up the scale,
        // else we look down first.
        const int multiplier = (note / 1.f) > 0.5 ? 1 : -1;
        float output = 0.f;
        // If note is 3.55, it's closer to 4 than 3, but closer to 3 than 5.
        // Try four, then 3, then 5, then 2, etc. Be sure to circle around, not
        // overflow the buffer.
        if (notes.at(index)) {
            output = octave + index * semitone;
            found = true;
        }
        // If no notes are currently active the CV output will never update...
        while (!found)
        {
            int d = index + distance * multiplier;
            // Is std::array::at a better choice here?
            // Our search for the next active note wraps around the scale, but
            // it should go up an octave then ...
            int access = d % notes.size();
            if (notes[access])
            {
                output = octave + access * semitone;
                found = true;
                break;
            }
            int negative_access = -d % notes.size();
            if (notes[negative_access])
            {
                output = octave + negative_access * semitone;
                found = true;
                break;
            }
            distance++;
        }

        patch.WriteCvOut(CV_OUT_1, output);
    }
}
