#!/bin/bash
# Convenience script to run visualization

echo "========================================================================"
echo "DISASTER RELIEF VISUALIZATION RUNNER"
echo "========================================================================"
echo ""

# Check if venv exists
if [ ! -d "venv" ]; then
    echo "❌ Virtual environment not found!"
    echo "Creating virtual environment..."
    python3 -m venv venv
    echo "✅ Virtual environment created"
    echo ""
    echo "Installing dependencies..."
    source venv/bin/activate
    pip install matplotlib numpy pillow --quiet
    echo "✅ Dependencies installed"
else
    echo "✅ Virtual environment found"
fi

echo ""

# Check if JSON files exist
if [ ! -f "sample_input.json" ] || [ ! -f "sample_output.json" ]; then
    echo "⚠️  JSON files not found. Running disaster_relief first..."
    ./disaster_relief sample
    echo ""
fi

echo "Running visualization..."
echo ""

# Activate venv and run visualization
source venv/bin/activate
python visualize.py

echo ""
echo "========================================================================"
echo "DONE! Check the generated files:"
echo "  - solution_visualization.png"
echo "  - solution_animated.gif"
echo "========================================================================"

