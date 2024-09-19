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
  const first = React.useRef(true);
  React.useEffect(() => {
    if (!first.current) return;
    first.current = false;
    setInterval(() => {
      setFps(fpsRef.current);
    }, 500)

    // setInterval(() => {
    //   client('/engine/get_primitives').then(r => {
    //     console.log(r.data);
    //   })
    // }, 1000)

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

  const [penOptions, setPenOptions] = React.useState<{
    color: {
      r: number;
      g: number;
      b: number;
      a: number;
    },
    width: number,
    type: 0 | 1 | 2 | 3,
    dash: number,
  }>({
    color: { r: 255, g: 255, b: 255, a: 255 },
    width: 1,
    type: 0,
    dash: 1,
  });

  const changePenOptionsTimeout = React.useRef<number | null>(null);
  React.useEffect(() => {
    if (changePenOptionsTimeout.current) clearTimeout(changePenOptionsTimeout.current);
    changePenOptionsTimeout.current = setTimeout(() => client('/engine/draw', {
      data: {
        PenOptions: penOptions,
      }
    }), 100);
  }, [penOptions])

  const [hideToolbar, setHideToolbar] = React.useState(false);
  const [toolbarHeight, setToolbarHeight] = React.useState(0);
  React.useEffect(() => {
    setToolbarHeight((document.getElementById('drawFuncs')?.clientHeight || 0) - 12);
  }, [])

  return (<>
    <div id="mousePosition">Engine: {engine_name}; FPS: {fps}{/*; {loading ? 'Loading...' : 'Ready.'}*/}.</div>
    <div id="drawFuncs-wrapper">
      <div id="drawFuncs"
        style={{
          height: (hideToolbar && toolbarHeight) ? 0 : (toolbarHeight || undefined),
          padding: hideToolbar ? '0 4px' : undefined,
        }}>
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
        <div id="pen-options">
          <div className="option no-text">
            颜色
            <input
              type="color"
              value={`#${penOptions.color.r.toString(16).padStart(2, '0')}${penOptions.color.g.toString(16).padStart(2, '0')}${penOptions.color.b.toString(16).padStart(2, '0')}`}
              onChange={e => {
                const color = e.target.value;
                setPenOptions({
                  ...penOptions,
                  color: {
                    r: parseInt(color.slice(1, 3), 16),
                    g: parseInt(color.slice(3, 5), 16),
                    b: parseInt(color.slice(5, 7), 16),
                    a: penOptions.color.a,
                  }
                })
              }}
            />
          </div>
          <div className="option no-text">
            透明
            <input
              type="range" min="0" max="255"
              value={penOptions.color.a}
              onChange={e => {
                setPenOptions({
                  ...penOptions,
                  color: {
                    ...penOptions.color,
                    a: parseInt(e.target.value)
                  }
                })
              }}
              style={{
                '--color': `rgba(${penOptions.color.r}, ${penOptions.color.g}, ${penOptions.color.b}, ${penOptions.color.a / 255})`
              } as React.CSSProperties}
            />
          </div>
          <div className="option">
            线宽
            <input
              type="number"
              value={penOptions.width}
              onChange={e =>
                setPenOptions({
                  ...penOptions,
                  width: parseInt(e.target.value)
                })
              }
            />
          </div>
          <div className="option">
            线性
            <select
              value={penOptions.type}
              onChange={e =>
                setPenOptions({
                  ...penOptions,
                  type: parseInt(e.target.value) as 0 | 1 | 2 | 3,
                })
              }>
              <option value={0}>实线</option>
              <option value={1}>虚线</option>
              <option value={2}>点线</option>
              <option value={3}>点划线</option>
            </select>
          </div>
          {
            penOptions.type > 0 &&
            <div className="option">
              虚长
              <input
                type="number"
                value={penOptions.dash}
                onChange={e =>
                  setPenOptions({
                    ...penOptions,
                    dash: parseInt(e.target.value)
                  })
                }
              />
            </div>
          }
        </div>
      </div>
      <button id="toolbar-control" onClick={() => setHideToolbar(!hideToolbar)}>
        {hideToolbar ? '展开' : '收起'}工具栏
      </button>
    </div >
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