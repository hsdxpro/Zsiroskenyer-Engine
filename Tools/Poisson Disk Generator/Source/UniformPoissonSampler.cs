using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.DirectX;

namespace PoissonDiskGen
{
    // http://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf

    class UniformPoissonSampler
    {
        public const int DefaultPointsPerIteration = 30;
        static readonly float SquareRootTwo = (float)Math.Sqrt(2);
        static readonly float TwoPI = (float)(Math.PI * 2);
        static readonly Random Random = new Random();

        struct Settings
        {
            public float MinimumDistance;
            public float MinimumDistanceSquared;
            public float CellSize;
            public float InverseCellSize;
            public Size GridSize;
        }

        struct State
        {
            Vector2?[,] Grid;
            List<Vector2> ActivePoints;
        }

        // ------------------------------------------------------------------
        static public List<Vector2> SampleRectangle(float width, float height, float minimumDistance)
        {
            return SampleRectangle(width, height, minimumDistance, DefaultPointsPerIteration);
        }

        // ------------------------------------------------------------------
        static public List<Vector2> SampleRectangle(float width, float height, float minimumDistance, int pointsPerIteration)
        {
            float cellSize = minimumDistance / SquareRootTwo;
            Settings settings = new Settings
            {
                MinimumDistance = minimumDistance,
                MinimumDistanceSquared = minimumDistance * minimumDistance,
                CellSize = cellSize,
                InverseCellSize = 1.0f / cellSize
                GridSize = new Size((int)(width / cellSize + 0.5f))
            };

            State state = new State
            {
                Grid = new Vector2?[settings.GridWidth, settings.GridHeight],
            };

            List<Vector2> activePoints = new List<Vector2>();

            return activePoints;
        }

        // ------------------------------------------------------------------
        static Vector2 GenerateAround(Vector2 center, float minimumDistance)
        {
            float radius = minimumDistance + (float)Random.NextDouble() * minimumDistance;
            float angle = (float)Random.NextDouble() * TwoPI;
            
            Vector2 point;
            point.X = radius * (float)Math.Sin(angle) + center.X;
            point.Y = radius * (float)Math.Cos(angle) + center.Y;

            return point;
        }


        // ------------------------------------------------------------------
        static float Distance(Vector2 v0, Vector2 v1)
        {
            return (float)Math.Sqrt(DistanceSquared(v0, v1));
        }

        // ------------------------------------------------------------------
        static float DistanceSquared(Vector2 v0, Vector2 v1)
        {
            return (v1.X - v0.X) * (v1.Y - v0.Y);
        }

    }
}
