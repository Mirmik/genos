#include <drivers/gpio/gpio.h>
#include <defs/gpiodefs.h>
#include <igris/util/bits.h>
#include <assert.h>

#include <asm/gpio.h>

void gpio_write(struct gpio_regs* g, gpio_mask_t mask, unsigned char level)
{
	stm32_gpio_write(g, mask, level);
}

gpio_mask_t gpio_read(struct gpio_regs* g, gpio_mask_t mask)
{
	return stm32_gpio_read(g, mask);
}

void gpio_toggle(struct gpio_regs* g, gpio_mask_t mask)
{
	stm32_gpio_toggle(g, mask);
}

int gpio_settings(struct gpio_regs * gpio, gpio_mask_t mask, uint32_t mode)
{
	int mode_val = 0;

	if ((mode & GPIO_MODE_OUT_SECTION) &&
	        (mode & GPIO_MODE_IN_SECTION))   /* mode is incorrect */
	{
		return -1;
	}

	if (mode & GPIO_MODE_INPUT)
	{
		mode_val = 0b00;

		if (mode & GPIO_MODE_IN_NOPULL)
		{
			bits_masked_assign_multimap(gpio->PUPDR, mask, 0b00, 2);
		}

		else if (mode & GPIO_MODE_IN_PULL_UP)
		{
			bits_masked_assign_multimap(gpio->PUPDR, mask, 0b01, 2);
		}

		else if (mode & GPIO_MODE_IN_PULL_DOWN)
		{
			bits_masked_assign_multimap(gpio->PUPDR, mask, 0b10, 2);
		}
	}
	
	else if (mode & GPIO_MODE_OUTPUT)
	{
		mode_val = 0b01;

		if (mode & GPIO_MODE_OUT_PUSH_PULL)
		{
			bits_clr(gpio->OTYPER, mask);
		}

		else if (mode & GPIO_MODE_OUT_OPEN_DRAIN)
		{
			bits_set(gpio->OTYPER, mask);
		}
	}
	
	else if (mode & GPIO_MODE_ALTERNATE)
	{
		mode_val = 0b10;
	}

	bits_masked_assign_multimap(gpio->MODER, mask, mode_val, 2);

	return 0;
}