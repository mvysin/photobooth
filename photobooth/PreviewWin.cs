using System;
using System.Drawing;
using System.Windows.Forms;

namespace Photobooth
{
    class PreviewWin : FullScreenWin
    {
        PictureBox pb;

        public PreviewWin()
        {
            Text = Application.ProductName + " Preview";
            ClientSize = new Size(600, 600*3/4);


            pb = new PictureBox();
            pb.Dock = DockStyle.Fill;
            pb.SizeMode = PictureBoxSizeMode.Zoom;
            Controls.Add(pb);
        }

        public PictureBox Picture
        {
            get { return pb; }
        }
    }
}