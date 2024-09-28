import * as React from 'react';
import { backend_endpoint, engine_name, engine_fps, client, retry_max_times } from './client';
import drawFuncs from './drawFuncs';

import './App.css';
import { IPoint } from './types';
import { randomEngineName } from './utils';

function getHeightUnfold(dom: HTMLElement) {
  const fakeNode = dom.cloneNode(true) as HTMLElement;
  fakeNode.style.position = 'absolute';
  dom.parentNode?.insertBefore(fakeNode, dom);
  fakeNode.style.height = 'auto';
  fakeNode.style.visibility = 'hidden';

  const height = fakeNode.getBoundingClientRect().height;
  dom.parentNode?.removeChild(fakeNode);

  return height;
}

function getPointDistance(p1: IPoint, p2: IPoint) {
  return Math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2);
}

export default function App() {
  const coordinateRef = React.useRef({ x: 0, y: 0 });
  const [coordinate, setCoordinate] = React.useState({ x: 0, y: 0 });
  const canvasRef = React.useRef<HTMLCanvasElement>(null);
  const [, setLoading] = React.useState(true);
  const loadingRef = React.useRef(true);
  const fpsRef = React.useRef(0);
  const [fps, setFps] = React.useState(0);
  const first = React.useRef(true);
  const [start, setStart] = React.useState(true);
  const startTimeoutRef = React.useRef<number | null>(null);
  const [errorMessage, setErrorMessage] = React.useState('等待后端响应...');
  React.useEffect(() => {
    if (!first.current) return;
    first.current = false;

    // setInterval(() => {
    //   client('/engine/get_primitives').then(r => {
    //     console.log(r.data);
    //   })
    // }, 1000)

    const width = document.body.clientWidth;
    const height = document.body.clientHeight;
    slidingWindowRef.current = {
      top_left: { x: 0, y: 0 },
      bottom_right: { x: width, y: height },
    }
    setSlidingWindow(slidingWindowRef.current)

    startTimeoutRef.current = setTimeout(() => {
      setStart(false);
    }, 100);

    const ConnectToBackend = (retryTimes = 0) => new Promise((resolve, reject) => {
      client('/engine/create', {
        data: {
          name: engine_name,
          width,
          height,
        },
        timeout: 1000,
      }).then(r => {
        console.log(r.data);
        console.log(`Engine ${engine_name} created with ${width}x${height}`);
        setInterval(() => {
          setFps(fpsRef.current);
          if (fpsRef.current === 0) {
            window.location.reload();
          }
        }, 500)
        resolve(1);
      }).catch(e => {
        console.error(e);
        setErrorMessage(`后端无响应，重新尝试连接.. ${retryTimes + 1}/${retry_max_times}`);
        if (retryTimes >= retry_max_times) {
          reject('Failed to create engine');
          return;
        } else {
          setTimeout(() => {
            ConnectToBackend(retryTimes + 1).then(resolve).catch(reject);
          }, 1000)
        }
      });
    });

    ConnectToBackend().then(() => {
      const ws = new WebSocket(`//${backend_endpoint}/engine/${engine_name}`);
      ws.onopen = function () {
        ws.send(`set_fps ${engine_fps}`);
        setLoading(false);
        if (startTimeoutRef.current) clearTimeout(startTimeoutRef.current);
        setStart(true);
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
      setErrorMessage("后端无响应，请检查后端是否正常运行。");
      // alert("Failed to create engine");
    });
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
    if (clickingSlidingWindowPoints) {
      setClickingSlidingWindowPoints(false);
      const top_left = {
        x: Math.min(pointers[0].x, pointers[1].x),
        y: Math.min(pointers[0].y, pointers[1].y),
      }
      const bottom_right = {
        x: Math.max(pointers[0].x, pointers[1].x),
        y: Math.max(pointers[0].y, pointers[1].y),
      }
      slidingWindowRef.current = { top_left, bottom_right };
      setSlidingWindow(slidingWindowRef.current);
      setClickedPoints([]);
      handleSlidingWindowChanged();
      return;
    }
    loadingRef.current = true;
    setLoading(true);
    currentDrawFunc.current.draw({ pointers })
      .then(() => drawingRef.current = true)
      .catch((e: string) => {
        console.error(e);
        loadingRef.current = false;
        setLoading(false);
        setClickedPoints([]);
        setErrorMessage(e);
        setStart(false);
      })
  }

  const drawingRef = React.useRef(false);
  const draggingRef = React.useRef(false);
  const [dragging, setDragging] = React.useState(false);
  const handleMouseDown = (e: React.MouseEvent<HTMLCanvasElement>, manual = false) => {
    if (!manual) {
      // canvas 点击事件
      // 'handleClick' handles clicking add points
      if (!clickingSlidingWindowPoints && currentDrawFunc.current.drawingMethod === 'click') return;
      // handle dragging add points & clicking sliding window points
      else {
        clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
        setClickedPoints(clickedPointsRef.current);
      }
    }
    if (clickingSlidingWindowPoints) {
      // 处理圈画裁剪框的点击
      if (clickedPointsRef.current.length === 2) {
        handleDraw();
      }
    } else if (currentDrawFunc.current.drawingMethod === 'drag') {
      // drag 的绘制在 handleMouseUp 中处理
      draggingRef.current = true;
      setDragging(true);
    } else if (clickedPointsRef.current.length === currentDrawFunc.current.requiredPointers && !currentDrawFunc.current.multiplePoints) {
      // 指定点数的绘制方法，达要求后直接绘制
      // 多点绘制方法在 handleDoubleClick 和 handleClick 中处理
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

  const handleClick = (e: React.MouseEvent<HTMLCanvasElement>) => {
    // handleClick 仅处理 click 类型的绘制方法，drag 类型的绘制方法和圈画裁剪框在 handleMouseDown 中处理
    if (clickingSlidingWindowPoints) return;
    if (currentDrawFunc.current.drawingMethod === 'drag') return;
    if (currentDrawFunc.current.multiplePoints && clickedPointsRef.current.length >= currentDrawFunc.current.requiredPointers && getPointDistance(coordinate, clickedPointsRef.current[0]) < 10) {
      const shadowPoint = { ...clickedPointsRef.current[0] };
      clickedPointsRef.current.push(shadowPoint);
      setClickedPoints(clickedPointsRef.current);
      handleDraw();
    } else {
      clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
      setClickedPoints(clickedPointsRef.current);
      handleMouseDown(e, true);
    }
  }

  const handleRightClick = (e: React.MouseEvent<HTMLCanvasElement>) => {
    e.preventDefault();
    clickedPointsRef.current.pop();
    setClickedPoints(clickedPointsRef.current);
    return;
  }

  const handleDoubleClick = () => {
    if (!currentDrawFunc.current.multiplePoints) return;
    clickedPointsRef.current.pop();
    setClickedPoints(clickedPointsRef.current);
    if (clickedPointsRef.current.length >= currentDrawFunc.current.requiredPointers)
      handleDraw();
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
    fill_color: {
      r: number;
      g: number;
      b: number;
      a: number;
    },
  }>({
    color: { r: 255, g: 255, b: 255, a: 255 },
    width: 1,
    type: 0,
    dash: 1,
    fill_color: { r: 0, g: 0, b: 0, a: 0 },
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

  const slidingWindowRef = React.useRef<{
    top_left: IPoint,
    bottom_right: IPoint,
  }>({
    top_left: { x: 0, y: 0 },
    bottom_right: { x: 0, y: 0 },
  })
  const [slidingWindow, setSlidingWindow] = React.useState<{
    top_left: IPoint,
    bottom_right: IPoint,
  }>({
    top_left: { x: 0, y: 0 },
    bottom_right: { x: 0, y: 0 },
  });
  const [slidingWindowMoving, setSlidingWindowMoving] = React.useState(false);

  const [clickingSlidingWindowPoints, setClickingSlidingWindowPoints] = React.useState<boolean>(false);

  const [enableSlidingWindow, setEnableSlidingWindow] = React.useState(false);
  const handleSlidingWindowChanged = (enable?: boolean) => {
    const enabledSliding = enable === undefined ? enableSlidingWindow : enable;
    const currentSlidingWindow = slidingWindowRef.current;
    if (currentSlidingWindow.top_left.x > currentSlidingWindow.bottom_right.x) {
      const temp = currentSlidingWindow.top_left.x;
      currentSlidingWindow.top_left.x = currentSlidingWindow.bottom_right.x;
      currentSlidingWindow.bottom_right.x = temp;
    }
    if (currentSlidingWindow.top_left.y > currentSlidingWindow.bottom_right.y) {
      const temp = currentSlidingWindow.top_left.y;
      currentSlidingWindow.top_left.y = currentSlidingWindow.bottom_right.y;
      currentSlidingWindow.bottom_right.y = temp;
    }
    client('/engine/set_global_options', {
      data: {
        GlobalOptions: {
          background_color: {
            r: 0,
            g: 0,
            b: 0,
            a: 255,
          },
          clip: {
            enable: enabledSliding,
            window: {
              Rectangle: currentSlidingWindow,
            },
            algorithm: 1,
          },
        },
      },
    });
  }

  const [hideToolbar, setHideToolbar] = React.useState(false);
  const [toolbarHeight, setToolbarHeight] = React.useState(0);
  React.useEffect(() => {
    if (hideToolbar) return;
    setToolbarHeight(getHeightUnfold((document.getElementById('drawFuncs')!)) - 8);
  }, [hideToolbar, penOptions, enableSlidingWindow])

  const previewSlidingWindowTimeout = React.useRef<number | null>(null);

  return (<>
    <div id="hover" style={{ opacity: start ? 0 : 1 }}>
      <div>{errorMessage}</div>
    </div>
    <div id="mousePosition">
      Engine: {engine_name}<button id="switch-engine-name" onClick={() => {
        randomEngineName();
        window.location.reload();
      }}>换</button>;
      FPS: {fps}{/*; {loading ? 'Loading...' : 'Ready.'}*/}.</div>
    <div id="slidingWindow">
      {
        ((enableSlidingWindow ? [
          {
            name: 'top-left',
            x: slidingWindow.top_left.x,
            y: slidingWindow.top_left.y,
          },
          {
            name: 'top-right',
            x: slidingWindow.bottom_right.x,
            y: slidingWindow.top_left.y,
          },
          {
            name: 'bottom-left',
            x: slidingWindow.top_left.x,
            y: slidingWindow.bottom_right.y,
          },
          {
            name: 'bottom-right',
            x: slidingWindow.bottom_right.x,
            y: slidingWindow.bottom_right.y,
          }
        ] : []) as (IPoint & { name: string })[]).map((point, index) => {
          return (
            <div
              key={index}
              className='point'
              style={{
                left: point.x,
                top: point.y
              }}
            >
              <div className='point-item'>
                <div className='point-circle'
                  onMouseDown={e => {
                    e.preventDefault();
                    setSlidingWindowMoving(true);
                    const handleMouseMove = (e: MouseEvent) => {
                      const currentSlidingWindow = { ...slidingWindowRef.current };
                      const target = point.name.split('-');
                      if (target[0] === 'top') {
                        currentSlidingWindow.top_left.y = e.clientY;
                      } else {
                        currentSlidingWindow.bottom_right.y = e.clientY;
                      }
                      if (target[1] === 'left') {
                        currentSlidingWindow.top_left.x = e.clientX;
                      } else {
                        currentSlidingWindow.bottom_right.x = e.clientX;
                      }
                      if (currentSlidingWindow.top_left.x > currentSlidingWindow.bottom_right.x) {
                        const temp = currentSlidingWindow.top_left.x;
                        currentSlidingWindow.top_left.x = currentSlidingWindow.bottom_right.x;
                        currentSlidingWindow.bottom_right.x = temp;
                      }
                      if (currentSlidingWindow.top_left.y > currentSlidingWindow.bottom_right.y) {
                        const temp = currentSlidingWindow.top_left.y;
                        currentSlidingWindow.top_left.y = currentSlidingWindow.bottom_right.y;
                        currentSlidingWindow.bottom_right.y = temp;
                      }
                      if (currentSlidingWindow.top_left.x < 0) {
                        currentSlidingWindow.top_left.x = 0;
                      }
                      if (currentSlidingWindow.top_left.y < 0) {
                        currentSlidingWindow.top_left.y = 0;
                      }
                      if (currentSlidingWindow.bottom_right.x > document.body.clientWidth) {
                        currentSlidingWindow.bottom_right.x = document.body.clientWidth;
                      }
                      if (currentSlidingWindow.bottom_right.y > document.body.clientHeight) {
                        currentSlidingWindow.bottom_right.y = document.body.clientHeight;
                      }
                      setSlidingWindow(currentSlidingWindow);
                      if (previewSlidingWindowTimeout.current) clearTimeout(previewSlidingWindowTimeout.current);
                      previewSlidingWindowTimeout.current = setTimeout(() => {
                        handleSlidingWindowChanged();
                      }, 100);
                    }
                    const handleMouseUp = (e: MouseEvent) => {
                      if (previewSlidingWindowTimeout.current) clearTimeout(previewSlidingWindowTimeout.current);
                      slidingWindowRef.current = slidingWindow;
                      setCoordinate({ x: e.clientX, y: e.clientY });
                      setSlidingWindowMoving(false);
                      handleSlidingWindowChanged();
                      document.removeEventListener('mousemove', handleMouseMove);
                      document.removeEventListener('mouseup', handleMouseUp);
                    }
                    document.addEventListener('mousemove', handleMouseMove);
                    document.addEventListener('mouseup', handleMouseUp);
                  }}
                  onDoubleClick={() => {
                    setSlidingWindow({
                      top_left: { x: 0, y: 0 },
                      bottom_right: { x: document.body.clientWidth, y: document.body.clientHeight },
                    })
                  }}
                />
                <p className='point-text top'>
                  {point.name}
                </p>
                <p className='point-text'>
                  ({point.x}, {point.y})
                </p>
              </div>
            </div>
          )
        })
      }
      <div id="slidingWindowBorder" style={{
        left: slidingWindow.top_left.x,
        top: slidingWindow.top_left.y,
        width: slidingWindow.bottom_right.x - slidingWindow.top_left.x,
        height: slidingWindow.bottom_right.y - slidingWindow.top_left.y,
        display: enableSlidingWindow ? 'block' : 'none',
      }} />
    </div>
    <div id="drawFuncs-wrapper">
      <div id="drawFuncs"
        style={{
          height: (hideToolbar && toolbarHeight) ? 0 : toolbarHeight,
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
          <div className="option no-text">
            填充
            <input
              type="color"
              value={`#${penOptions.fill_color.r.toString(16).padStart(2, '0')}${penOptions.fill_color.g.toString(16).padStart(2, '0')}${penOptions.fill_color.b.toString(16).padStart(2, '0')}`}
              onChange={e => {
                const color = e.target.value;
                setPenOptions({
                  ...penOptions,
                  fill_color: {
                    r: parseInt(color.slice(1, 3), 16),
                    g: parseInt(color.slice(3, 5), 16),
                    b: parseInt(color.slice(5, 7), 16),
                    a: penOptions.fill_color.a,
                  }
                })
              }}
            />
          </div>
          <div className="option no-text">
            透明
            <input
              type="range" min="0" max="255"
              value={penOptions.fill_color.a}
              onChange={e => {
                setPenOptions({
                  ...penOptions,
                  fill_color: {
                    ...penOptions.fill_color,
                    a: parseInt(e.target.value)
                  }
                })
              }}
              style={{
                '--color': `rgba(${penOptions.fill_color.r}, ${penOptions.fill_color.g}, ${penOptions.fill_color.b}, ${penOptions.fill_color.a / 255})`
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
        <button onClick={() => {
          const newValue = !enableSlidingWindow;
          setEnableSlidingWindow(newValue);
          handleSlidingWindowChanged(newValue);
        }}>
          {enableSlidingWindow ? '关闭' : '开启'}裁剪框
        </button>
        {
          enableSlidingWindow && (
            <button onClick={() => {
              setClickedPoints([]);
              clickedPointsRef.current = [];
              setClickingSlidingWindowPoints(true)
            }}>
              圈画裁剪框
            </button>
          )
        }
      </div>
      <button id="toolbar-control" onClick={() => setHideToolbar(!hideToolbar)}>
        {hideToolbar ? '展开' : '收起'}工具栏
      </button>
    </div>
    {
      ([...clickedPoints, { ...slidingWindowMoving ? [] : coordinate, type: clickingSlidingWindowPoints ? 'sliding' : dragging ? 'drag' : (currentDrawFunc.current.multiplePoints && clickedPoints.length >= currentDrawFunc.current.requiredPointers && getPointDistance(coordinate, clickedPoints[0]) < 10) ? 'ending' : 'current' }] as IPoint[]).filter(point => point.x && point.y)
        .map((point, index) => {
          return (
            <div
              className={`point ${point.type}`}
              key={index}
              style={{
                left: point.x,
                top: point.y
              }}
            >
              <div className='point-item'>
                <div className='point-circle'
                  style={{
                    backgroundColor: (point.type === 'sliding' || point.type === 'ending') ? 'transparent' : point.type === 'drag' ? 'yellow' : point.type === 'current' ? 'blue' : 'red'
                  }}
                />
                <p className='point-text top'>
                  {
                    clickingSlidingWindowPoints ? (index === 0 ? 'top-left' : 'bottom-right') :
                      point.type === 'ending' ? '结束' : (!currentDrawFunc.current.multiplePoints && (index + 1) === currentDrawFunc.current.requiredPointers) ? '结束' : (index + 1)
                  }
                </p>
                <p className='point-text'>
                  ({point.x}, {point.y})
                </p>
              </div>
            </div>
          )
        })
    }
    <canvas
      width={0}
      height={0}
      ref={canvasRef}
      onMouseDown={handleMouseDown}
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
      onClick={handleClick}
      onDoubleClick={handleDoubleClick}
      onContextMenu={handleRightClick}
    />
  </>)
}
