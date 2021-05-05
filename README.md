# openGL_rendering

Using this scripts need to GLM, GL, GLFW.

And can change other images in Archive

# When the resolution is 512

![512](https://user-images.githubusercontent.com/37854961/117104335-eb674980-adb6-11eb-8e34-b218a2fea908.jpg)

# When the resolution is 1024

![1024](https://user-images.githubusercontent.com/37854961/117104340-ec987680-adb6-11eb-96ab-e6f523ccb46a.jpg)

# When the resolution is 2048

![2048](https://user-images.githubusercontent.com/37854961/117104341-edc9a380-adb6-11eb-88ce-b7999591ddde.jpg)

# If increase the bias coefficient
The degree of shadow spreading has become smaller, as if the shadow is coming in vertically.
The more I grew the coefficient, the less I could see the shadow.

![bias](https://user-images.githubusercontent.com/37854961/117104352-f1f5c100-adb6-11eb-83b0-9e776d99f71d.jpg)

# When using 64 samples
Compared to 4 samples, it came out very smoothly and the soft shadow came out neatly.

![64sample](https://user-images.githubusercontent.com/37854961/117104349-f0c49400-adb6-11eb-9816-f30bdb1a6d6c.jpg)

# Increasing the coefficient multiplied by the poisson disk
The shadow softens a lot.

![poisson_disk_high](https://user-images.githubusercontent.com/37854961/117104361-f4581b00-adb6-11eb-9367-aba3d89578bb.jpg)

# Decreasing the coefficient multiplied by the poisson disk
The shadow appeared hard.

![poisson_disk_low](https://user-images.githubusercontent.com/37854961/117104366-f5894800-adb6-11eb-9121-f241d1e7f712.jpg)
