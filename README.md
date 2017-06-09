# SLICO
This code performs the SLICO algorithm plus connected components using opencv

The Original code by...:

Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Süsstrunk, SLIC Superpixels Compared to State-of-the-art Superpixel Methods, IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 34, num. 11, p. 2274 - 2282, May 2012.

Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Süsstrunk, SLIC Superpixels, EPFL Technical Report no. 149300, June 2010.

...in http://ivrl.epfl.ch/research/superpixels was slightly modified to get the seeds' coordinates.

In this approach, it's assumed a rectangular 'pseudo-grid' of size: rows-8 , cols-10. Number of superPixels=80. So the connected components algorithm will only execute if the SLIC algorithms detects 80 superpixels. This can be modified though. :) so no worries.

This was tested using Ubuntu 14.08 in the Eclipse IDE.


EXAMPLE

test image



after SLIC

![slic](https://user-images.githubusercontent.com/11812560/26956005-629282f0-4c80-11e7-9131-d56e5b5c9e46.png)
