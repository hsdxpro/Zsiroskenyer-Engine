namespace PoissonDiskGen
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.previewPanel = new System.Windows.Forms.Panel();
            this.generateButton = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.pointsPerIterationCtl = new System.Windows.Forms.NumericUpDown();
            this.minimumDistanceCtl = new System.Windows.Forms.NumericUpDown();
            this.poissonCoefficients = new System.Windows.Forms.TextBox();
            this.pointsGeneratedLbl = new System.Windows.Forms.Label();
            this.displayBoundingVolumes = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.pointsPerIterationCtl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.minimumDistanceCtl)).BeginInit();
            this.SuspendLayout();
            // 
            // previewPanel
            // 
            this.previewPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.previewPanel.Location = new System.Drawing.Point(12, 25);
            this.previewPanel.Name = "previewPanel";
            this.previewPanel.Size = new System.Drawing.Size(300, 300);
            this.previewPanel.TabIndex = 0;
            this.previewPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.previewPanel_Paint);
            // 
            // generateButton
            // 
            this.generateButton.Location = new System.Drawing.Point(318, 331);
            this.generateButton.Name = "generateButton";
            this.generateButton.Size = new System.Drawing.Size(221, 47);
            this.generateButton.TabIndex = 5;
            this.generateButton.Text = "Regenerate!";
            this.generateButton.UseVisualStyleBackColor = true;
            this.generateButton.Click += new System.EventHandler(this.generateButton_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 361);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(97, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Points per iteration:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 333);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(94, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Minimum distance:";
            // 
            // pointsPerIterationCtl
            // 
            this.pointsPerIterationCtl.Location = new System.Drawing.Point(112, 359);
            this.pointsPerIterationCtl.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.pointsPerIterationCtl.Name = "pointsPerIterationCtl";
            this.pointsPerIterationCtl.Size = new System.Drawing.Size(44, 20);
            this.pointsPerIterationCtl.TabIndex = 2;
            this.pointsPerIterationCtl.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // minimumDistanceCtl
            // 
            this.minimumDistanceCtl.DecimalPlaces = 2;
            this.minimumDistanceCtl.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.minimumDistanceCtl.Location = new System.Drawing.Point(112, 331);
            this.minimumDistanceCtl.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.minimumDistanceCtl.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            131072});
            this.minimumDistanceCtl.Name = "minimumDistanceCtl";
            this.minimumDistanceCtl.Size = new System.Drawing.Size(44, 20);
            this.minimumDistanceCtl.TabIndex = 1;
            this.minimumDistanceCtl.Value = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            // 
            // poissonCoefficients
            // 
            this.poissonCoefficients.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.poissonCoefficients.Location = new System.Drawing.Point(318, 25);
            this.poissonCoefficients.Multiline = true;
            this.poissonCoefficients.Name = "poissonCoefficients";
            this.poissonCoefficients.ReadOnly = true;
            this.poissonCoefficients.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.poissonCoefficients.Size = new System.Drawing.Size(221, 300);
            this.poissonCoefficients.TabIndex = 4;
            // 
            // pointsGeneratedLbl
            // 
            this.pointsGeneratedLbl.AutoSize = true;
            this.pointsGeneratedLbl.Location = new System.Drawing.Point(9, 9);
            this.pointsGeneratedLbl.Name = "pointsGeneratedLbl";
            this.pointsGeneratedLbl.Size = new System.Drawing.Size(35, 13);
            this.pointsGeneratedLbl.TabIndex = 6;
            this.pointsGeneratedLbl.Text = "label1";
            // 
            // displayBoundingVolumes
            // 
            this.displayBoundingVolumes.AutoSize = true;
            this.displayBoundingVolumes.Checked = true;
            this.displayBoundingVolumes.CheckState = System.Windows.Forms.CheckState.Checked;
            this.displayBoundingVolumes.Location = new System.Drawing.Point(165, 332);
            this.displayBoundingVolumes.Name = "displayBoundingVolumes";
            this.displayBoundingVolumes.Size = new System.Drawing.Size(147, 17);
            this.displayBoundingVolumes.TabIndex = 3;
            this.displayBoundingVolumes.Text = "Display bounding spheres";
            this.displayBoundingVolumes.UseVisualStyleBackColor = true;
            this.displayBoundingVolumes.CheckedChanged += new System.EventHandler(this.displayBoundingVolumes_CheckedChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(551, 390);
            this.Controls.Add(this.displayBoundingVolumes);
            this.Controls.Add(this.pointsGeneratedLbl);
            this.Controls.Add(this.poissonCoefficients);
            this.Controls.Add(this.pointsPerIterationCtl);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.generateButton);
            this.Controls.Add(this.previewPanel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.minimumDistanceCtl);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.Text = "Poisson Disk Generator";
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyDown);
            ((System.ComponentModel.ISupportInitialize)(this.pointsPerIterationCtl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.minimumDistanceCtl)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel previewPanel;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button generateButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown pointsPerIterationCtl;
        private System.Windows.Forms.NumericUpDown minimumDistanceCtl;
        private System.Windows.Forms.TextBox poissonCoefficients;
        private System.Windows.Forms.Label pointsGeneratedLbl;
        private System.Windows.Forms.CheckBox displayBoundingVolumes;
    }
}

