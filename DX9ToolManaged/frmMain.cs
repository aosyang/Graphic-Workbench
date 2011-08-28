using System;
using System.Windows.Forms;
using Microsoft.DirectX.Direct3D;

namespace DX9ToolManaged
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            devicePanel.Init(this);
        }

        private DX9MeshBufferManagedWrapper mesh = new DX9MeshBufferManagedWrapper();

        private void devicePanel_OnCreateDevice(object sender, DeviceEventArgs e)
        {
            mesh.CreateFromFile("../Data/Mesh/Berserker.emd");
        }

        private void devicePanel_OnRender(object sender, DeviceEventArgs e)
        {
            e.Device.SetupView();
            mesh.Render();
        }

    }
}
