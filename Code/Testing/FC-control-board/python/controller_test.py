# %% import
import numpy as np
import matplotlib.pyplot as plt

import pygame
import serial

pygame.joystick.quit()
pygame.quit()

#
pygame.init()
pygame.joystick.init()
joysticks = [pygame.joystick.Joystick(x) for x in range(pygame.joystick.get_count())]

print(joysticks[0].get_name())
# Print the joystick's state
print(joysticks[0].get_init())
print(joysticks[0].get_id())
print(joysticks[0].get_numaxes())
print(joysticks[0].get_numbuttons())
print(joysticks[0].get_numhats())

print(joysticks[0].get_axis(0))


def get_joystick_values(joystick):
    # Return values for all axes
    axis_index = [0, 1, 3, 4]
    axes = [joystick.get_axis(i) for i in axis_index]
    # Mask out some axes
    return axes

def send_joystick_serial(ser, values):
    # Values are float between -1 and 1
    # Convert to int between 0 and 512
    values = np.array(values)
    values = values * 256 + 256
    values = values.astype(int)
    # Send values
    # ser.write(values)
    # Print values
    print(values)


# Display the joystick's state
# Open pygame window
size = [500, 700]
screen = pygame.display.set_mode(size)

# Set title to the window
pygame.display.set_caption("Joystick Tester")

# Loop until the user clicks the close button.
done = False
while done == False:
    # If user clicked close
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True

    # Clear the screen and set the screen background
    screen.fill((0, 0, 0))

    # Get joystick axes
    joystick_values = get_joystick_values(joysticks[0])
    send_joystick_serial(None, joystick_values)

    # Draw axes
    for i in range(len(joystick_values)):
        pygame.draw.rect(screen, (255, 255, 255), [25 + 50 * i, 25, 20, int(200 * joystick_values[i] + 100)])

    # Update the screen
    pygame.display.flip()

    # Limit fps
    fps = 60
    pygame.time.delay(int(1000 / fps))

pygame.joystick.quit()
pygame.quit()

