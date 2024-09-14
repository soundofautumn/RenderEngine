import * as React from 'react';
import { backend_endpoint, engine_name, fps, client } from './client';
import { drawLine } from './drawFuncs';

import './App.css';
import { IPoint } from './types';

export default function App() {
  const coordinateRef = React.useRef({ x: 0, y: 0 });
  const [coordinate, setCoordinate] = React.useState({ x: 0, y: 0 });
  const canvasRef = React.useRef<HTMLCanvasElement>(null);
  const [loading, setLoading] = React.useState(true);
  const loadingRef = React.useRef(true);
  React.useEffect(() => {
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
        ws.send(`set_fps ${fps}`);
        setLoading(false);
        loadingRef.current = false;
      };
      ws.binaryType = "arraybuffer";
      ws.onmessage = function (event) {
        if (!canvasRef.current) return;
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
            clickedPointsRef.current = [];
            setClickedPoints([]);
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
  const currentDrawFunc = React.useRef(drawLine);
  const drawingRef = React.useRef(false);
  const handleMouseDown = (e: React.MouseEvent<HTMLCanvasElement>) => {
    if (drawingRef.current) return;
    clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
    setClickedPoints(clickedPointsRef.current);
    if (clickedPointsRef.current.length === currentDrawFunc.current.requiredPointers) {
      loadingRef.current = true;
      setLoading(true);
      currentDrawFunc.current.draw(clickedPointsRef.current)
        .then(() => drawingRef.current = true)
        .catch((e: string) => {
          console.error(e);
          clickedPointsRef.current = [];
          drawingRef.current = false;
          loadingRef.current = false;
          setLoading(false);
          setClickedPoints([]);
        })
    }
  }

  return (<>
    <div id="mousePosition">X: {coordinate.x}, Y: {coordinate.y}; Engine: {engine_name}; {loading ? 'Loading...' : 'Ready.'}</div>
    {
      clickedPoints.map((point, index) => {
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
              <div className='point-circle' />
              <p className='point-text'>
                ({point.x}, {point.y})
              </p>
            </div>
          </div>
        )
      })
    }
    <canvas ref={canvasRef} onMouseDown={handleMouseDown} onMouseMove={handleMouseMove} />
  </>)
}