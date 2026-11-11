#ifndef NEOGEOPOCKET_HEADER
#define NEOGEOPOCKET_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/// This runs all save state functions for each chip.
int packState(void *statePtr);

/// This runs all load state functions for each chip.
void unpackState(const void *statePtr);

/// Gets the total state size in bytes.
int getStateSize(void);

/// Prepare the selected border in the inactive VRAM slot.
void setupEmuBackground(void);
/// Publish the prepared border before the VBlank palette transfer.
void publishEmuBackground(void);
unsigned getBorderMode(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !NEOGEOPOCKET_HEADER
