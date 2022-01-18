#pragma once
namespace Eloquent {
    namespace ML {
        namespace Port {
            class SEFR {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        return dot(x,   0.051812415184  , -0.112512256175  , 0.1642848592  , 0.024818980072  , -0.112479645455  , 0.163380679039 ) <= 0.06507825635687405 ? 0 : 1;
                    }

                protected:
                    /**
                    * Compute dot product
                    */
                    float dot(float *x, ...) {
                        va_list w;
                        va_start(w, 6);
                        float dot = 0.0;

                        for (uint16_t i = 0; i < 6; i++) {
                            const float wi = va_arg(w, double);
                            dot += x[i] * wi;
                        }

                        return dot;
                    }
                };
            }
        }
    }
