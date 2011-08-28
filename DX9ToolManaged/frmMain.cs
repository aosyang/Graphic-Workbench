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
            //D3DXVECTOR3 vEyePt = new D3DXVECTOR3( -5.0f, 5.0f, -5.0f );
            //D3DXVECTOR3 vLookatPt = new D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
            //D3DXVECTOR3 vUpVec = new D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

            //e.Device.SetViewMatrix(vEyePt, vLookatPt, vUpVec);
            mesh.Render();
        }

    }
}
