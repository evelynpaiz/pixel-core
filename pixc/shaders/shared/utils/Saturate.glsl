/**
 * Saturate a value within the range [0, 1].
 *
 * @param x Input value to be saturated.
 *
 * @return Saturated value within the range [0, 1].
 */
float saturate(float x) {
    // Clamp the input value between the minimum of 0 and the maximum of 1
    return clamp(x, 0.0f, 1.0f);
}
