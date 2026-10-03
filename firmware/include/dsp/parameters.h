#ifndef DSP_PARAMETERS_H
#define DSP_PARAMETERS_H

struct DelayParameters {
    int time_ms;
    float feedback;
    float wet;
};

struct ReverbParameters {
    float room_size;
    float damping;
    float wet;
};

struct ChorusParameters {
    float rate_hz;
    float depth;
    float base_ms;
    float wet;
    bool wet_filter;
};

extern DelayParameters currentDelayParams;
extern ReverbParameters currentReverbParams;
extern ChorusParameters currentChorusParams;

#endif // DSP_PARAMETERS_H