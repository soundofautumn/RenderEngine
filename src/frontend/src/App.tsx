import * as React from 'react';
import { backend_endpoint, engine_name, engine_fps, client } from './client';
import drawFuncs from './drawFuncs';

import './App.css';
import { IPoint } from './types';

export default function App() {
  const coordinateRef = React.useRef({ x: 0, y: 0 });
  const [coordinate, setCoordinate] = React.useState({ x: 0, y: 0 });
  const canvasRef = React.useRef<HTMLCanvasElement>(null);
  const [loading, setLoading] = React.useState(true);
  const loadingRef = React.useRef(true);
  const fpsRef = React.useRef(0);
  const [fps, setFps] = React.useState(0);
  React.useEffect(() => {
    setInterval(() => {
      setFps(fpsRef.current);
    }, 500)

    const width = document.body.clientWidth;
    const height = document.body.clientHeight;
    client('/engine/create', {
      data: {
        name: engine_name,
        width,
        height,
      }
    }).then(r => {
      console.log(r.data);
      console.log(`Engine ${engine_name} created with ${width}x${height}`);
      const ws = new WebSocket(`ws://${backend_endpoint}/engine/${engine_name}`);
      ws.onopen = function () {
        ws.send(`set_fps ${engine_fps}`);
        setLoading(false);
        loadingRef.current = false;
      };
      ws.binaryType = "arraybuffer";
      ws.onmessage = function (event) {
        if (!canvasRef.current) return;

        fpsRef.current += 1;
        setTimeout(() => {
          fpsRef.current -= 1;
        }, 1000)

        const ctx = canvasRef.current.getContext("2d");
        canvasRef.current.width = width;
        canvasRef.current.height = height;
        const data = new Uint8ClampedArray(event.data);

        const imageData = new ImageData(data, width, height);
        if (ctx) {
          ctx.putImageData(imageData, 0, 0);

          if (drawingRef.current && loadingRef.current) {
            setLoading(false)
            loadingRef.current = false;
            drawingRef.current = false;
            setClickedPoints(clickedPointsRef.current);
          }
        }
      };
    }).catch(e => {
      console.error(e);
      alert("Failed to create engine");
    })
  }, [])

  const handleMouseMove = (e: React.MouseEvent<HTMLCanvasElement>) => {
    coordinateRef.current = { x: e.clientX, y: e.clientY };
    setCoordinate(coordinateRef.current);
  }

  const clickedPointsRef = React.useRef<IPoint[]>([]);
  const [clickedPoints, setClickedPoints] = React.useState<IPoint[]>([]);
  const currentDrawFunc = React.useRef(drawFuncs[0].drawFunc);
  const [currentDrawFuncIndex, setCurrentDrawFuncIndex] = React.useState(0);
  const handleDraw = () => {
    const pointers = clickedPointsRef.current;
    clickedPointsRef.current = [];
    loadingRef.current = true;
    setLoading(true);
    currentDrawFunc.current.draw({ pointers })
      .then(() => drawingRef.current = true)
      .catch((e: string) => {
        console.error(e);
        loadingRef.current = false;
        setLoading(false);
        setClickedPoints([]);
      })
  }

  const drawingRef = React.useRef(false);
  const draggingRef = React.useRef(false);
  const [dragging, setDragging] = React.useState(false);
  const handleMouseDown = (e: React.MouseEvent<HTMLCanvasElement>) => {
    clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
    setClickedPoints(clickedPointsRef.current);
    if (currentDrawFunc.current.drawingMethod === 'drag') {
      draggingRef.current = true;
      setDragging(true);
    } else if (clickedPointsRef.current.length === currentDrawFunc.current.requiredPointers) {
      handleDraw();
    }
  }
  const handleMouseUp = (e: React.MouseEvent<HTMLCanvasElement>) => {
    if (draggingRef.current) {
      draggingRef.current = false;
      setDragging(false);
      clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
      setClickedPoints(clickedPointsRef.current);
      handleDraw();
    }
  }

  return (<>
    <div id="mousePosition">Engine: {engine_name}; X: {coordinate.x}, Y: {coordinate.y}; fps: {fps}; {loading ? 'Loading...' : 'Ready.'}</div>
    <div id="drawFuncs">
      {
        drawFuncs.map((drawFunc, index) => {
          return (
            <button
              key={index}
              onClick={() => {
                currentDrawFunc.current = drawFunc.drawFunc;
                setCurrentDrawFuncIndex(index);
              }}
              disabled={index === currentDrawFuncIndex}
            >
              {drawFunc.name}
            </button>
          )
        })
      }
    </div>
    {
      ([...clickedPoints, { ...coordinate, type: dragging ? 'drag' : 'current' }] as IPoint[])
        .map((point, index) => {
          return (
            <div
              className='point'
              key={index}
              style={{
                left: point.x,
                top: point.y
              }}
            >
              <div className='point-item'>
                <div className='point-circle'
                  style={{
                    backgroundColor: point.type === 'drag' ? 'yellow' : point.type === 'current' ? 'blue' : 'red'
                  }}
                />
                <p className='point-text'>
                  ({point.x}, {point.y})
                </p>
              </div>
            </div>
          )
        })
    }
    <canvas
      ref={canvasRef}
      onMouseDown={handleMouseDown}
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}

    />
  </>)
}