using System;
using System.Drawing;
using System.Windows.Forms;

using Microsoft.DirectX;

namespace DX9ToolManaged
{
    public partial class DevicePanel : UserControl
    {
        #region Members

        // The DirectX device
        //private Device _device = null;

        private DX9DeviceWrapper deviceWrapper = new DX9DeviceWrapper();

        // The Form to place the DevicePanel onto
        private Form _parent = null;

        // On this event we can start to render our scene
        public event RenderEventHandler OnRender;

        // Now we know that the device is created
        public event CreateDeviceEventHandler OnCreateDevice;

        #endregion

        #region ctor

        public DevicePanel()
        {
            InitializeComponent();
        }

        #endregion

        #region Init DX

        /// <summary>
        /// Init the DirectX-Stuff here
        /// </summary>
        /// <param name="parent">parent of the DevicePanel</param>
        public void Init(Form parent)
        {
            try
            {
                _parent = parent;

                //// Setup our D3D stuff
                //PresentParameters presentParams = new PresentParameters();
                //presentParams.Windowed = true;
                //presentParams.SwapEffect = SwapEffect.Discard;
                //_device = new Device(0, DeviceType.Hardware, this,
                //CreateFlags.HardwareVertexProcessing, presentParams);

                deviceWrapper.Init(this.Handle);

                if (OnCreateDevice != null)
                    OnCreateDevice(this, new DeviceEventArgs(ref deviceWrapper));

                parent.Show();
                parent.Focus();

                while (Created)
                {
                    Render();
                    Application.DoEvents();
                }

            }
            catch (DirectXException ex)
            {
                throw new ArgumentException("Error initializing DirectX", ex);
            }
        }

        #endregion

        #region Properties

        /// <summary>
        /// Extend this list of properties if you like
        /// </summary>
        private Color _deviceBackColor = Color.Black;

        public Color DeviceBackColor
        {
            get { return _deviceBackColor; }
            set { _deviceBackColor = value; }
        }


        #endregion

        #region Rendering

        /// <summary>
        /// Rendering-method
        /// </summary>
        public void Render()
        {
            deviceWrapper.Clear();

            deviceWrapper.BeginScene();

            // Render of scene here
            if (OnRender != null)
                OnRender(this, new DeviceEventArgs(ref deviceWrapper));

            deviceWrapper.EndScene();
            deviceWrapper.Present();
        }

        #endregion

        #region Overrides

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);

            deviceWrapper.SetWindowSize(Size.Width, Size.Height);
            deviceWrapper.ResetDevice();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            if (!DesignMode)
            {
                // Render on each Paint
                this.Render();
            }
            else
            {
                base.OnPaint(e);
            }
        }

        protected override void OnKeyPress(System.Windows.Forms.KeyPressEventArgs e)
        {
            // Close on escape
            if ((int)(byte)e.KeyChar == (int)System.Windows.Forms.Keys.Escape)
                _parent.Close();
        }

        #endregion
    }

    public delegate void RenderEventHandler(object sender, DeviceEventArgs e);
    public delegate void CreateDeviceEventHandler(object sender, DeviceEventArgs e);

    public class DeviceEventArgs : EventArgs
    {
        private DX9DeviceWrapper _deviceWrapper;

        public DX9DeviceWrapper Device
        {
            get
            {
                return _deviceWrapper;
            }
        }

        public DeviceEventArgs(ref DX9DeviceWrapper wrapper)
        {
            _deviceWrapper = wrapper;
        }
    }
}
