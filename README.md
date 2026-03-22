# Qt program to explore Fourier Descriptors

If you have a polygon of $n$ vertices, you can take the $(x_i,y_i)$ coordinates of its vertices and convert them in complex numbers $z_i = x_i + jy_i$ for all $0 \leq i < n$.

If you take the DFT of the sequence so obtained, you get the so-called Fourier Descriptors, which give you information about the polygon.

With this program you can explore the Fourier Descriptors.

The GUI works as follows:

* Left click: add a point
* Right click: remove a point
* Scroll: zoom the view
* Shift-scroll: rotate the polygon
* Ctrl-scroll: resize the polygon
* Numbers 3-9: add a polygon of 3-9 vertices
* `c`: clear the scene
* `u`: make the current polygon regular

Also the DFT coefficients can be modified from the GUI, the polygon gets updated accordingly. Right-click on a DFT coefficient sets it to zero. Left bars are real parts of the coefficients, right bars are the immaginary parts.

