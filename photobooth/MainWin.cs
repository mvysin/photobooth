using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace Photobooth
{
    class MainWin : FullScreenWin
    {
        WPD.PortableDevice cam;
        WPD.StillImageObject stillImageObj;
        WPD.StorageObject storageObj;

        MemoryStream img;
        bool taking;
        string imgFileName, imgSaveFolder;
        List<WPD.StorageItem> stgItems;

        PreviewWin preview;
        HashSet<string> objects;
        PictureBox pb;
        TableLayoutPanel panel;
        ComboBox cbCamera;
        Button btnRefresh, btnTake, btnSave, btnClear;

        Timer captureTimeout;


        public MainWin()
        {
            Text = Application.ProductName;
            ClientSize = new Size(600, 600*3/4);

            preview = new PreviewWin();

            cam = null;
            stillImageObj = null;
            storageObj = null;
            img = null;
            taking = false;
            imgSaveFolder = Environment.GetFolderPath(Environment.SpecialFolder.Personal);

            captureTimeout = new Timer();
            captureTimeout.Interval = 1000;
            captureTimeout.Tick += new EventHandler(CaptureTimeout);

            panel = new TableLayoutPanel();
            //panel.CellBorderStyle = TableLayoutPanelCellBorderStyle.Single;
            panel.Dock = DockStyle.Fill;
            Controls.Add(panel);

            cbCamera = new ComboBox();
            cbCamera.DropDownStyle = ComboBoxStyle.DropDownList;
            cbCamera.SelectedIndexChanged += new EventHandler(CameraChanged);
            panel.Controls.Add(cbCamera, 0, 0);

            btnRefresh = new Button();
            btnRefresh.Text = "Refresh";
            btnRefresh.FlatStyle = FlatStyle.System;
            btnRefresh.Click += new EventHandler(RefreshDevicesOnClick);
            panel.Controls.Add(btnRefresh, 1, 0);

            btnTake = new Button();
            btnTake.Text = "Take Picture";
            btnTake.FlatStyle = FlatStyle.System;
            btnTake.Click += new EventHandler(TakePicture);
            panel.Controls.Add(btnTake, 2, 0);

            btnSave = new Button();
            btnSave.Text = "Save Picture";
            btnSave.FlatStyle = FlatStyle.System;
            btnSave.Click += new EventHandler(SavePicture);
            panel.Controls.Add(btnSave, 3, 0);

            btnClear = new Button();
            btnClear.Text = "Clear Display";
            btnClear.FlatStyle = FlatStyle.System;
            btnClear.Click += new EventHandler(ClearPicture);
            panel.Controls.Add(btnClear, 4, 0);

            pb = new PictureBox();
            pb.Dock = DockStyle.Fill;
            pb.SizeMode = PictureBoxSizeMode.Zoom;
            pb.DoubleClick += new EventHandler(TakePicture);
            panel.Controls.Add(pb, 0, 1);
            panel.SetColumnSpan(pb, 5);

            panel.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            panel.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));

            RefreshDevices();
            RefreshButtons();
        }

        void CaptureTimeout(object sender, EventArgs e)
        {
            captureTimeout.Stop();

            if (taking)
            {
                taking = false;
                RefreshButtons();
            }
        }

        void CameraChanged(object sender, EventArgs e)
        {
            cam = cbCamera.SelectedItem as WPD.PortableDevice;
            if (cam == null)
            {
                stillImageObj = null;
                storageObj = null;
                return;
            }

            cam.DeviceRemoved += new WPD.PortableDeviceEventHandler(DevRemoved);
            cam.DeviceReset += new WPD.PortableDeviceEventHandler(DevReset);
            cam.ObjectAdded += new WPD.PortableDeviceEventHandler(ObjAdded);
            cam.ObjectRemoved += new WPD.PortableDeviceEventHandler(ObjRemoved);
            cam.StorageFormat += new WPD.PortableDeviceEventHandler(StgFormat);

            WPD.PortableDeviceCategory stillImageCat = cam.GetCategory(WPD.FunctionCategory.StillImageCapture);
            stillImageObj = stillImageCat.Functions[0] as WPD.StillImageObject;

            WPD.PortableDeviceCategory storageCat = cam.GetCategory(WPD.FunctionCategory.Storage);
            storageObj = storageCat.Functions[0] as WPD.StorageObject;

            CreateInitialState();
            RefreshButtons();
        }

        void ClearPicture(object sender, EventArgs e)
        {
            preview.Picture.Image = null;
        }

        protected override void OnLoad(EventArgs e)
        {
            preview.Location = Location + new Size(100, 100);
            preview.StartPosition = FormStartPosition.Manual;
            preview.Show();

            base.OnLoad(e);
        }

        void StgFormat(object sender, WPD.PortableDeviceEventArgs e)
        {
            storageObj.Refresh();
            CreateInitialState();
        }

        void ObjRemoved(object sender, WPD.PortableDeviceEventArgs e)
        {
            storageObj.Refresh();
            CreateInitialState();
        }

        void ObjAdded(object sender, WPD.PortableDeviceEventArgs e)
        {
            storageObj.Refresh();
            PictureTaken();
        }

        void DevReset(object sender, WPD.PortableDeviceEventArgs e)
        {
            WPD.PortableDeviceCategory stillImageCat = cam.GetCategory(WPD.FunctionCategory.StillImageCapture);
            stillImageObj = stillImageCat.Functions[0] as WPD.StillImageObject;

            WPD.PortableDeviceCategory storageCat = cam.GetCategory(WPD.FunctionCategory.Storage);
            storageObj = storageCat.Functions[0] as WPD.StorageObject;

            CreateInitialState();
            RefreshButtons();
        }

        void DevRemoved(object sender, WPD.PortableDeviceEventArgs e)
        {
            stillImageObj = null;
            storageObj = null;
            cam = null;

            RefreshDevices();
        }

        void CreateInitialState()
        {
            objects = new HashSet<string>();
            foreach (WPD.StorageItem i in storageObj.Items)
                CreateInitialStateHelper(i);
        }

        void CreateInitialStateHelper(WPD.StorageItem item)
        {
            objects.Add(item.Id);
            foreach (WPD.StorageItem i in item.Items)
                CreateInitialStateHelper(i);
        }

        WPD.StorageItem[] GetNewItems(bool commit)
        {
            List<WPD.StorageItem> items = new List<WPD.StorageItem>();
            foreach (WPD.StorageItem i in storageObj.Items)
                GetNewItemsHelper(items, i, commit);

            return items.ToArray();
        }

        void GetNewItemsHelper(List<WPD.StorageItem> items, WPD.StorageItem item, bool commit)
        {
            if (!objects.Contains(item.Id))
            {
                items.Add(item);
                if (commit)
                    objects.Add(item.Id);
            }

            foreach (WPD.StorageItem i in item.Items)
                GetNewItemsHelper(items, i, commit);
        }

        void PictureTaken()
        {
            captureTimeout.Stop();
            taking = false;

            WPD.StorageItem[] newItems = GetNewItems(true);
            if (newItems.Length > 0)
            {
                List<string> imageNames = new List<string>();
                foreach (WPD.StorageItem i in newItems)
                {
                    string name = Path.GetFileNameWithoutExtension(i.Name);
                    if (!imageNames.Contains(name))
                        imageNames.Add(name);
                }

                imageNames.Sort();

                imgFileName = imageNames[imageNames.Count-1];
                stgItems = new List<WPD.StorageItem>();

                foreach (WPD.StorageItem i in newItems)
                {
                    // only consider the last one
                    string name = Path.GetFileNameWithoutExtension(i.Name);
                    if (name != imgFileName)
                        continue;

                    stgItems.Add(i);

                    // only transfer the jpeg version
                    if (Path.GetExtension(i.Name).ToLowerInvariant() != ".jpg")
                        continue;

                    if (img != null)
                        img.Close();

                    img = new System.IO.MemoryStream();
                    i.TransferTo(img);

                    Image imgObject = Image.FromStream(img);
                    pb.Image = imgObject;
                    preview.Picture.Image = imgObject;
                }
            }

            RefreshButtons();
        }

        void RefreshButtons()
        {
            btnSave.Enabled = !taking && cam != null && img != null;
            btnTake.Enabled = !taking && cam != null;
        }

        void RefreshDevices()
        {
            cbCamera.Items.Clear();

            WPD.PortableDevice curDev = null;
            int curDevIndex = 0;

            WPD.PortableDeviceManager mgr = new WPD.PortableDeviceManager();
            IList<WPD.PortableDevice> devs = mgr.DevicesFilteredBy(WPD.FunctionCategory.StillImageCapture);
            if (devs.Count == 0)
                cbCamera.Items.Add("<No Camera>");
            foreach (WPD.PortableDevice d in devs)
            {
                cbCamera.Items.Add(d);
                if (cam != null &&
                    d.Id == cam.Id && d.Name == cam.Name &&
                    d.Manufacturer == cam.Manufacturer && d.Description == cam.Description)
                {
                    curDev = d;
                    curDevIndex = cbCamera.Items.Count-1;
                }
            }

            Size sz = cbCamera.GetPreferredSize(Size.Empty);
            sz.Width = Math.Max(sz.Width, 200);
            cbCamera.Size = sz;

            cbCamera.SelectedIndex = curDevIndex;

            RefreshButtons();
        }

        void RefreshDevicesOnClick(object sender, EventArgs e)
        {
            RefreshDevices();
        }

        void SavePicture(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.AddExtension = true;
            sfd.Filter = "JPEG Files (*.jpg)|*.jpg|All Files (*.*)|*.*";
            sfd.InitialDirectory = imgSaveFolder;
            sfd.OverwritePrompt = true;
            sfd.SupportMultiDottedExtensions = true;
            sfd.Title = "Save Image As";
            sfd.FileName = imgFileName;

            DialogResult dr = sfd.ShowDialog(this);
            if (dr == DialogResult.Cancel)
                return;

            imgSaveFolder = Path.GetDirectoryName(sfd.FileName);
            string fileBaseName = Path.GetFileNameWithoutExtension(sfd.FileName);

            foreach (WPD.StorageItem i in stgItems)
            {
                MemoryStream ms = null;
                if (Path.GetExtension(i.Name).ToLowerInvariant() == ".jpg")
                {
                    // the jpeg version is already transferred - in "img
                    ms = img;
                }
                else
                {
                    ms = new MemoryStream();
                    i.TransferTo(ms);
                }

                FileStream fs = new FileStream(Path.Combine(imgSaveFolder, fileBaseName + Path.GetExtension(i.Name)), FileMode.Create, FileAccess.Write, FileShare.None);
                fs.Write(ms.GetBuffer(), 0, (int)ms.Length);
                fs.Close();

                if (Path.GetExtension(i.Name).ToLowerInvariant() != ".jpg")
                    ms.Close();
            }
        }

        void TakePicture(object sender, EventArgs e)
        {
            if (cam == null)
                return;

            taking = true;
            pb.Image = null;
            preview.Picture.Image = null;
            RefreshButtons();
            Update();

            stillImageObj.Capture();

            captureTimeout.Start();
        }
    }
}