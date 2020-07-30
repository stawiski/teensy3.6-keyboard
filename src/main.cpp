#include <Arduino.h>
#include <Mouse.h>

#define RNG_CR_GO_MASK          0x1U
#define RNG_CR_HA_MASK          0x2U
#define RNG_CR_INTM_MASK        0x4U
#define RNG_CR_CLRI_MASK        0x8U
#define RNG_CR_SLP_MASK         0x10U
#define RNG_SR_OREG_LVL_MASK    0xFF00U
#define RNG_SR_OREG_LVL_SHIFT   8
#define RNG_SR_OREG_LVL(x)      (((uint32_t)(((uint32_t)(x)) << RNG_SR_OREG_LVL_SHIFT)) & RNG_SR_OREG_LVL_MASK)
#define SIM_SCGC6_RNGA          ((uint32_t)0x00000200)

#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof(arr[0]))

uint32_t trng(void)
{
    /* Wait until RNG is ready. */
    while ((RNG_SR & RNG_SR_OREG_LVL(0xF)) == 0);
    return RNG_OR;
}

void setup(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    /* Configure RNG peripheral. */
    SIM_SCGC6 |= SIM_SCGC6_RNGA;
    RNG_CR &= ~RNG_CR_SLP_MASK;
    RNG_CR |= RNG_CR_HA_MASK;
    RNG_CR |= RNG_CR_GO_MASK;
}

float getTrueRandNormalized(void)
{
    uint32_t rand = trng() >> 16;
    /* Return normalized random number between 0.0f to 1.0f. */
    return ((float)rand) / (float)UINT16_MAX;
}

void delayRandom(float maxMs)
{
    /* Delay up to maxMs depending on randomness. */
    delay((uint32_t)(maxMs * getTrueRandNormalized()));
}

void loop(void)
{
    /* Delay random time up to 500 ms. */
    delayRandom(500.0f);

    float x = getTrueRandNormalized(), y = getTrueRandNormalized();

    x -= 0.5f;
    y -= 0.5f;

    x *= 5.0f;
    y *= 5.0f;

    Mouse.move((int8_t)x, (int8_t)y);

    /* Blink the LED. */
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
}
