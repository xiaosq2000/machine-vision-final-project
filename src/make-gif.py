
import os
import imageio


def create_gif(image_list, gif_name):

    frames = []
    for image_name in image_list:
        if image_name.endswith('.jpg'):
            print(image_name)
            frames.append(imageio.imread(image_name))
    # Save them as frames into a gif
    imageio.mimsave(gif_name, frames, 'GIF', duration=0.3)

    return


def main():

    path = r'./share/dst/'
    files = os.listdir(path)
    #files.sort()
    files.sort(key=lambda x: int(x[:-4]))

    image_list = [path+img for img in files]
    gif_name = r'./share/graphics/dst_gif.gif'
    create_gif(image_list, gif_name)


if __name__ == "__main__":
    main()
