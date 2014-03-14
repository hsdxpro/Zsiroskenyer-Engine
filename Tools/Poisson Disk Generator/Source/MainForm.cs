using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using Microsoft.DirectX;

namespace PoissonDiskGen
{
    public partial class MainForm : Form
    {
        private List<Vector2> CurrentPoints;
        private const int AboutID = 0x100;

        public MainForm()
        {
            InitializeComponent();

            try
            {
                SystemMenu sysMenu = SystemMenu.CreateFromForm(this);
                sysMenu.InsertSeparator(0);
                sysMenu.InsertMenu(0, AboutID, "About this tool...");
            }
            catch(NoSystemMenuException)
            {
            }

            generateButton_Click(null, null);
        }

        private void generateButton_Click(object sender, EventArgs e)
        {
            CurrentPoints = UniformPoissonDiskSampler.SampleCircle(new Vector2(0, 0), 1, 
                                (float)minimumDistanceCtl.Value, 
                                (int)pointsPerIterationCtl.Value);

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("{0} point{1} generated with a minimum distance of {2}:", 
                            CurrentPoints.Count, CurrentPoints.Count > 1 ? "s" : "",
                            (float)minimumDistanceCtl.Value);
            pointsGeneratedLbl.Text = sb.ToString();

            sb = new StringBuilder();            
            for (int i = 0; i < CurrentPoints.Count; ++i)
            {
                Vector2 p = CurrentPoints[i];
                sb.AppendFormat("{0}f, {1}f{2}", p.X, p.Y, i != CurrentPoints.Count-1 ? "," : "");
                sb.Append(Environment.NewLine);
            }
            poissonCoefficients.Text = sb.ToString();

            previewPanel.Invalidate();
        }

        private void displayBoundingVolumes_CheckedChanged(object sender, EventArgs e)
        {
            previewPanel.Invalidate();
        }

        static Pen blackPen = new Pen(Color.Black);
        static Pen whitePen = new Pen(Color.White);
        static Pen grayPen = new Pen(Color.Gray);
        static Brush whiteBrush = new SolidBrush(Color.White);
        static Brush alphaBrush = new SolidBrush(Color.FromArgb(80, Color.Gray));

        private void previewPanel_Paint(object sender, PaintEventArgs pe)
        {
            Graphics g = pe.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;

            int width = previewPanel.ClientRectangle.Size.Width;
            int height = previewPanel.ClientRectangle.Size.Height;

            g.Clip = new Region(pe.ClipRectangle);

            g.ResetTransform();
            g.Clear(Color.Black);
            g.TranslateTransform(width/2.0f, height/2.0f);

            float scale = Math.Min(width / 2, height / 2) * 0.75f;
            Pen unitCircleColor = displayBoundingVolumes.Checked ? whitePen : grayPen;
            g.DrawEllipse(unitCircleColor, -scale, -scale, scale*2, scale*2);

            float size = (float)minimumDistanceCtl.Value * 2;
            foreach(Vector2 p in CurrentPoints)
                DrawCircle(g, p * scale, size * scale);

            g.ResetTransform();
        }

        private void DrawCircle(Graphics g, Vector2 p, float s)
        {
            g.FillEllipse(whiteBrush, p.X - 1, p.Y - 1, 2, 2);
            g.DrawEllipse(whitePen, p.X-1, p.Y-1, 2, 2);

            if (displayBoundingVolumes.Checked)
            {
                g.FillEllipse(alphaBrush, (p.X - s / 2), (p.Y - s / 2), s, s);
                g.DrawEllipse(grayPen, (p.X - s/2), (p.Y - s/2), s, s);
            }
        }

        private void MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
                Close();
        }

        protected override void WndProc(ref Message msg)
        {
            if (msg.Msg == (int)WindowMessages.wmSysCommand)
            {
                switch (msg.WParam.ToInt32())
                {
                    case AboutID:
                    {
                        AboutBox ab = new AboutBox();
                        ab.ShowDialog();
                    } break;
                }
            }

            base.WndProc(ref msg);
        }
    }
}
