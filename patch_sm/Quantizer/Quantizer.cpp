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
    bool n[12] = { true, false, true, true, false, false, false, true, true, false, false, false };

    /** Initialize the patch_sm hardware object */
    patch.Init();

    /** Loop forever */
    while(1)
    {
        patch.ProcessAllControls();
        bool state = patch.gate_in_1.State();
        patch.SetLed(state);

        // What is the allowed range of output for this value? Is it different
        // for different CV inputs?

        // The voltage range for the CV inputs are +-pIn.
        // But I'm pretty sure the output of this call is +-1v. See
        // InitBipolarCV definition.
        const float voct_cv = patch.GetAdcValue(CV_5);
        // Keystep 37 (outputs 0v - 10v). This mapping perfectly aligns the
        // Keystep output with out CV output.
        float voct = fmap(voct_cv, 0.f, 5.f);
        //patch.WriteCvOut(CV_OUT_2, voct);

        // Now all notes are quantized to the 12-tone scale.
        const float semi = floorf(voct / semitone);
        int nx = static_cast<int>(semi) % 12;
        // Show if input voltage was in the current scale (after round)
        if (n[nx]) patch.WriteCvOut(CV_OUT_2, 5);
        else patch.WriteCvOut(CV_OUT_2, 0);
        // Only update CV output when a gate signal is received.
        if (!state) continue;
        float add = 0.f;
        // For now always go up to the next note. Later we'll find the nearest
        // note instead.
        while (!n[nx % 12])
        {
            ++nx;
            add += 1.f;
        }

        const float out = (semi + add) * semitone;
        patch.WriteCvOut(CV_OUT_1, out);
    }
}
