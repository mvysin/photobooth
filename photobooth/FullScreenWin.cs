using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Photobooth
{
    [StructLayout(LayoutKind.Sequential)]
    struct RECT
    {
        public int left;
        public int top;
        public int right;
        public int bottom;
    }

    enum WmSizingEdge
    {
        Left=1,
        Right,
        Top,
        TopLeft,
        TopRight,
        Bottom,
        BottomLeft,
        BottomRight
    }

    class FullScreenWin : Form
    {
        Size ncAdjust;
        Button btnRestore, btnMinimize, btnExit;

        public FullScreenWin()
        {
            ncAdjust = Size - ClientSize;
            BackColor = Color.Black;

            btnMinimize = new Button();
            InitWindowButton(btnMinimize);
            btnMinimize.Text = "\x30";
            Controls.Add(btnMinimize);

            btnRestore = new Button();
            InitWindowButton(btnRestore);
            btnRestore.Text = "\x32";
            Controls.Add(btnRestore);

            btnExit = new Button();
            InitWindowButton(btnExit);
            btnExit.Text = "\x72";
            Controls.Add(btnExit);
        }

        private bool CanShowWindowBtns
        {
            get { return WindowState == FormWindowState.Maximized; }
        }

        void Control_MouseMove(object sender, MouseEventArgs e)
        {
            Control c = (Control)sender;

            Point pt = c.PointToScreen(e.Location);
            pt = PointToClient(pt);

            MouseEventArgs e2 = new MouseEventArgs(e.Button, e.Clicks, pt.X, pt.Y, e.Delta);
            OnMouseMove(e2);
        }

        private void InitWindowButton(Button btn)
        {
            btn.Visible = false;
            btn.Size = new Size(35, 25);
            btn.Font = new Font("Marlett", 12f, FontStyle.Bold);
            btn.FlatStyle = FlatStyle.Popup;
            btn.ForeColor = Color.White;
            btn.Click += new EventHandler(WindowButtonClick);
        }

        protected override void OnControlAdded(ControlEventArgs e)
        {
            e.Control.MouseMove += new MouseEventHandler(Control_MouseMove);
            base.OnControlAdded(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (CanShowWindowBtns)
            {
                if (e.X > btnRestore.Left && e.Y < btnRestore.Bottom)
                    btnRestore.Visible = btnMinimize.Visible = btnExit.Visible = true;
                else
                    btnRestore.Visible = btnMinimize.Visible = btnExit.Visible = false;
            }
            base.OnMouseMove(e);
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            if (btnMinimize != null)
                btnMinimize.Location = new Point(ClientSize.Width-btnExit.Width-btnRestore.Width-btnMinimize.Width, 0);
            if (btnRestore != null)
                btnRestore.Location = new Point(ClientSize.Width-btnExit.Width-btnRestore.Width, 0);
            if (btnExit != null)
                btnExit.Location = new Point(ClientSize.Width-btnExit.Width, 0);
            base.OnSizeChanged(e);
        }

        void WindowButtonClick(object sender, EventArgs e)
        {
            if (sender == btnMinimize)
            {
                WindowState = FormWindowState.Minimized;
            }
            else if (sender == btnRestore)
            {
                WindowState = FormWindowState.Normal;
                FormBorderStyle = FormBorderStyle.Sizable;
                btnExit.Visible = btnRestore.Visible = btnMinimize.Visible = false;
            }
            else if (sender == btnExit)
            {
                Close();
            }
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == 0x0112)            // WM_SYSCOMMAND
            {
                if (((uint)m.WParam & 0x0FFF0) == 0xF030)       // SC_MAXIMIZE
                {
                    btnExit.Visible = btnRestore.Visible = btnMinimize.Visible = true;
                    FormBorderStyle = FormBorderStyle.None;
                }
                else if (((uint)m.WParam & 0x0FFF0) == 0xF120)  // SC_RESTORE
                {
                    if (WindowState == FormWindowState.Maximized)
                    {
                        btnExit.Visible = btnRestore.Visible = btnMinimize.Visible = false;
                        FormBorderStyle = FormBorderStyle.Sizable;
                    }
                }
            }
            else if (m.Msg == 0x0214)       // WM_SIZING
            {
                RECT rect = (RECT)Marshal.PtrToStructure(m.LParam, typeof(RECT));
                WmSizingEdge edge = (WmSizingEdge)m.WParam;

                if (edge == WmSizingEdge.Top || edge == WmSizingEdge.Bottom)
                {
                    int newHeight = rect.bottom - rect.top - ncAdjust.Height;
                    int newWidth = newHeight / 3 * 4;
                    if (edge == WmSizingEdge.Top)
                        rect.left = rect.right - newWidth - ncAdjust.Width;
                    else
                        rect.right = rect.left + newWidth + ncAdjust.Width;
                }
                else
                {
                    int newWidth = rect.right - rect.left - ncAdjust.Width;
                    int newHeight = newWidth / 4 * 3;
                    if (edge == WmSizingEdge.Left || edge == WmSizingEdge.TopLeft || edge == WmSizingEdge.TopRight)
                        rect.top = rect.bottom - newHeight - ncAdjust.Height;
                    else
                        rect.bottom = rect.top + newHeight + ncAdjust.Height;
                }

                Marshal.StructureToPtr(rect, m.LParam, false);
                m.Result = (IntPtr)1;
            }

            base.WndProc(ref m);
        }
    }
}
