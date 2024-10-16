import * as React from 'react';
import { backend_endpoint, engine_name, engine_fps, client, retry_max_times } from './client';
import drawFuncs from './drawFuncs';

import './App.css';
import { BOUNDER_OFFSET, IDrawApiParam, IPoint, IPrimitive, IShadowBounder } from './types';
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
        }, 500);
        fetchPrimitives();
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

  const handleMouseMove = (e: React.MouseEvent<HTMLCanvasElement> | React.MouseEvent<HTMLDivElement>) => {
    coordinateRef.current = { x: e.clientX, y: e.clientY };
    setCoordinate(coordinateRef.current);

    if (movingScalePoint && originScalePoint && originShadowBounder) {
      const center_point = shadowVertex?.find(point => point.type === 'center');
      if (!center_point) {
        console.log('Center point not found');
        return
      };
      // 以中心点为参考点计算缩放比例
      const offsetX = coordinate.x - center_point.x;
      const offsetY = coordinate.y - center_point.y;
      const offsetXOrigin = originScalePoint.x - center_point.x;
      const offsetYOrigin = originScalePoint.y - center_point.y;
      const scaleX = offsetX / offsetXOrigin;
      const scaleY = offsetY / offsetYOrigin;
      // 求所有顶点的偏移
      const computeOffset = (vertex: IPoint, xOffset: 1 | -1, yOffset: 1 | -1) => {
        return [
          vertex.x - center_point.x + xOffset * BOUNDER_OFFSET,
          vertex.y - center_point.y + yOffset * BOUNDER_OFFSET,
        ]
      }
      const [offsetXTopLeft, offsetYTopLeft] = computeOffset({ x: originShadowBounder!.left_bounder, y: originShadowBounder!.top_bounder }, -1, -1);
      const [offsetXTopRight, offsetYTopRight] = computeOffset({ x: originShadowBounder!.right_bounder, y: originShadowBounder!.top_bounder }, 1, -1);
      const [offsetXBottomRight, offsetYBottomRight] = computeOffset({ x: originShadowBounder!.right_bounder, y: originShadowBounder!.bottom_bounder }, 1, 1);
      const [offsetXBottomLeft, offsetYBottomLeft] = computeOffset({ x: originShadowBounder!.left_bounder, y: originShadowBounder!.bottom_bounder }, -1, 1);
      // 计算按照缩放比例后的顶点
      const new_bounder_points = {
        top_left: {
          x: center_point.x + offsetXTopLeft * scaleX,
          y: center_point.y + offsetYTopLeft * scaleY,
        },
        top_right: {
          x: center_point.x + offsetXTopRight * scaleX,
          y: center_point.y + offsetYTopRight * scaleY,
        },
        bottom_right: {
          x: center_point.x + offsetXBottomRight * scaleX,
          y: center_point.y + offsetYBottomRight * scaleY,
        },
        bottom_left: {
          x: center_point.x + offsetXBottomLeft * scaleX,
          y: center_point.y + offsetYBottomLeft * scaleY,
        },
      }
      setShadowVertex([
        ...shadowVertex!.filter(point => point.type !== 'bounder'),
        ...Object.values(new_bounder_points).map(point => ({ ...point, type: 'bounder' })) as IPoint[],
      ])
      const transformedBounder = {
        left_bounder: new_bounder_points.top_left.x,
        right_bounder: new_bounder_points.top_right.x,
        top_bounder: new_bounder_points.top_left.y,
        bottom_bounder: new_bounder_points.bottom_right.y,
      }
      setShadowBounder({
        left_bounder: Math.min(transformedBounder.left_bounder, transformedBounder.right_bounder) + BOUNDER_OFFSET,
        right_bounder: Math.max(transformedBounder.left_bounder, transformedBounder.right_bounder) - BOUNDER_OFFSET,
        top_bounder: Math.min(transformedBounder.top_bounder, transformedBounder.bottom_bounder) + BOUNDER_OFFSET,
        bottom_bounder: Math.max(transformedBounder.top_bounder, transformedBounder.bottom_bounder) - BOUNDER_OFFSET,
      })
    } else if (movingRotatePoint && rotateStartPoint) {
      const deltaX = coordinate.x - rotateStartPoint.x;
      const deltaY = coordinate.y - rotateStartPoint.y;
      const angleInRadians = Math.atan2(deltaY, deltaX);
      setAngle(angleInRadians);
    } else if (showingPrimitive && movingCenterPoint && shadowVertex) {
      const csv = [...shadowVertex];
      csv.find(point => point.type === 'center')!.x = coordinate.x;
      csv.find(point => point.type === 'center')!.y = coordinate.y;
      setShadowVertex(csv);
    } else if (translatingPrimitive && originShadowBounder && originShadowVertex) {
      const offsetX = coordinate.x - translateStartPoint!.x;
      const offsetY = coordinate.y - translateStartPoint!.y;
      const currentShadowBounder = { ...originShadowBounder };
      currentShadowBounder.left_bounder += offsetX;
      currentShadowBounder.right_bounder += offsetX;
      currentShadowBounder.top_bounder += offsetY;
      currentShadowBounder.bottom_bounder += offsetY;
      const currentShadowVertex = originShadowVertex.map(point => {
        return {
          x: point.x + offsetX,
          y: point.y + offsetY,
          type: point.type,
        }
      })
      setShadowBounder(currentShadowBounder);
      setShadowVertex(currentShadowVertex);
    } else if (currentEditingPoint === null) return;
    else if (!showingPrimitive) return;
    else if (currentEditingPoint.multiPoints) {
      const currentPrimitive = { ...showingPrimitive };
      // @ts-expect-error undefined
      currentPrimitive.params.find(p => p.type === "multi_points").value[currentEditingPoint.index] = { x: e.clientX, y: e.clientY };
      setShowingPrimitive(currentPrimitive);
    }
    else {
      const currentPrimitive = { ...showingPrimitive };
      currentPrimitive.params[currentEditingPoint.index].value = { x: e.clientX, y: e.clientY };
      setShowingPrimitive(currentPrimitive);
    }
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
      .then(() => {
        drawingRef.current = true;
        fetchPrimitives();
      })
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
    if (showingPrimitive) {
      return;
    } else if (clickingSlidingWindowPoints) {
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
    handleRotateMouseUp();
    if (draggingRef.current) {
      draggingRef.current = false;
      setDragging(false);
      clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
      setClickedPoints(clickedPointsRef.current);
      handleDraw();
    }
  }

  const handleClick = (e: React.MouseEvent<HTMLCanvasElement>) => {
    // handleClick 仅处理 click 类型的绘制方法，drag 类型的绘制方法、圈画裁剪框和修改图元的已有点在 handleMouseDown 中处理
    if (showingPrimitive) return;
    if (clickingSlidingWindowPoints) return;
    if (currentDrawFunc.current.drawingMethod === 'drag') return;
    if (currentDrawFunc.current.multiplePoints && clickedPointsRef.current.length >= currentDrawFunc.current.requiredPointers && getPointDistance(coordinate, clickedPointsRef.current[0]) < 10) {
      // const shadowPoint = { ...clickedPointsRef.current[0] };
      // clickedPointsRef.current.push(shadowPoint);
      // setClickedPoints(clickedPointsRef.current);
      handleDraw();
    } else {
      clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
      setClickedPoints(clickedPointsRef.current);
      handleMouseDown(e, true);
    }
  }

  const handleRightClick = (e: React.MouseEvent<HTMLCanvasElement>) => {
    e.preventDefault();
    if (showingPrimitive) return;
    clickedPointsRef.current.pop();
    setClickedPoints(clickedPointsRef.current);
    return;
  }

  const handleDoubleClick = () => {
    if (showingPrimitive) return;
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
    changePenOptionsTimeout.current = setTimeout(() => client('/engine/primitive/push_back', {
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
  const [disableToolbar, setDisableToolbar] = React.useState(false);
  const [toolbarHeight, setToolbarHeight] = React.useState(0);
  React.useEffect(() => {
    if (hideToolbar || disableToolbar) return;
    setToolbarHeight(getHeightUnfold((document.getElementById('drawFuncs')!)) - 8);
  }, [hideToolbar, disableToolbar, penOptions, enableSlidingWindow])

  const previewSlidingWindowTimeout = React.useRef<number | null>(null);

  const [primitives, setPrimitives] = React.useState<IPrimitive[]>([]);
  const [showingPrimitive, setShowingPrimitive] = React.useState<IPrimitive | null>(null);
  const fetchPrimitives = (update_index?: number) => {
    client("/engine/primitive/get_all").then(r => {
      const raw_primitives = r.data as { [apiEndpoint: string]: { [param: string]: object } }[];
      console.log(raw_primitives);
      const primitives = raw_primitives.map((raw_primitive, index) => {
        const apiEndpoint = Object.keys(raw_primitive)[0];
        const matchedDrawFuncs = drawFuncs.filter(df => df.drawFunc.apiEndpoint === apiEndpoint).map(df => ({
          params: df.drawFunc.params,
          type: df.drawFunc.type,
          names: df.drawFunc.params.map((param, index) => {
            if (param.type === 'point')
              return param.name || `p${index + 1}`;
            else if (param.type === 'func' && param.func)
              return param.name || `f${index + 1}`;
            else if (param.type === 'multi_points')
              return param.name || `mp${index + 1}`;
            else return param.name || `u${index + 1}`;
          })
        })).sort((a, b) => {
          const matchTimes = (df: {
            params: IDrawApiParam[];
            names: string[];
          }) => df.names.filter(name => Object.keys(raw_primitive[apiEndpoint]).includes(name)).length;
          return matchTimes(b) - matchTimes(a);
        });
        const matchedDrawFunc = matchedDrawFuncs.length > 0 ? matchedDrawFuncs[0] : null;
        const params:
          (IDrawApiParam & { value: object })[]
          = matchedDrawFunc?.params.map((param, index) => {
            const name = (() => {
              if (param.type === 'point')
                return param.name || `p${index + 1}`;
              else if (param.type === 'func' && param.func)
                return param.name || `f${index + 1}`;
              else if (param.type === 'multi_points')
                return param.name || `mp${index + 1}`;
              else return param.name || `u${index + 1}`;
            })();
            return {
              type: param.type,
              name,
              value: raw_primitive[apiEndpoint][name],
            }
          }) || Object.keys(raw_primitive[apiEndpoint]).map((name) => ({
            type: 'unknown',
            name,
            value: raw_primitive[apiEndpoint][name],
          }))
        return { apiEndpoint, params, index, type: matchedDrawFunc?.type || 'unknown', editable: matchedDrawFunc ? true : false };
      });
      console.log(primitives);
      setPrimitives(primitives);
      if (showingPrimitive && update_index) {
        const newShowingPrimitive = primitives.find(p => p.index === update_index);
        if (newShowingPrimitive) setShowingPrimitive(newShowingPrimitive);
      }
    })
  }
  React.useEffect(() => {
    document.getElementById('primitives-list')?.scrollTo({
      top: document.getElementById('primitives-list')?.scrollHeight,
      behavior: 'smooth',
    });
  }, [primitives])
  const [primitiveOpacity, setPrimitiveOpacity] = React.useState(0);
  React.useEffect(() => {
    if (showingPrimitive === null) {
      setDisableToolbar(false);
      setPrimitiveOpacity(0);
    } else {
      setDisableToolbar(true);
      setPrimitiveOpacity(1);
    }
  }, [showingPrimitive])
  const [movingPrimitivePoint, setMovingPrimitivePoint] = React.useState<boolean>(false);
  const [currentEditingPoint, setCurrentEditingPoint] = React.useState<{
    index: number;
    multiPoints?: boolean;
  } | null>(null);
  const handleEditPointMouseDown = (point: IPoint) => {
    if (!showingPrimitive) return;
    setMovingPrimitivePoint(true);
    const index = showingPrimitive.params.findIndex(param => param.type === 'point' && param.value.x === point.x && param.value.y === point.y);
    if (index === -1) {
      const multi_param = showingPrimitive.params.find(param => param.type === 'multi_points');
      if (!multi_param) {
        console.log('Multi points not found', showingPrimitive.params);
        setMovingPrimitivePoint(false);
        return;
      }
      else {
        const multiPoints = multi_param.value as IPoint[];
        const multiIndex = multiPoints.findIndex(p => p.x === point.x && p.y === point.y);
        if (multiIndex === -1) {
          console.log('Point not found', multiPoints);
          setMovingPrimitivePoint(false);
          return;
        } else {
          setCurrentEditingPoint({ index: multiIndex, multiPoints: true });
        }
      }
    } else {
      setCurrentEditingPoint({ index });
    }
  }
  const handleEditPointMouseUp = () => {
    setMovingPrimitivePoint(false);
    const currentPrimitive = { ...showingPrimitive };
    setCurrentEditingPoint(null);
    // 发一个请求
    client("/engine/primitive/modify", {
      data: {
        Primitive: {
          [currentPrimitive.apiEndpoint || "unknown"]: {
            type: currentPrimitive.type,
            ...Object.fromEntries((currentPrimitive.params || []).map((param) => [param.name || 'unknown', param.value])),
            algorithm: 0,
          }
        },
        Index: currentPrimitive.index,
      }
    })
  }

  const [shadowBounder, setShadowBounder] = React.useState<IShadowBounder | null>(null);
  const [shadowVertex, setShadowVertex] = React.useState<IPoint[] | null>([]);
  React.useEffect(() => {
    if (!showingPrimitive) {
      setShadowBounder(null);
      setShadowVertex(null);
      return;
    }
    const all_pointers: IPoint[] = showingPrimitive.params.map(param => (param.type === 'point' && param.value) ? ({ ...param.value, type: 'bounder' }) : undefined).filter(p => !!p) as IPoint[];
    const param_multi_points = showingPrimitive.params.find(param => param.type === 'multi_points');
    if (param_multi_points)
      all_pointers.push(...(param_multi_points.value as IPoint[]).map(point => ({ ...point, type: 'bounder' }) as IPoint));
    console.log('all_pointers', all_pointers);

    const left_bounder: number = Math.min(...all_pointers.map(p => p.x));
    const right_bounder: number = Math.max(...all_pointers.map(p => p.x));
    const top_bounder: number = Math.min(...all_pointers.map(p => p.y));
    const bottom_bounder: number = Math.max(...all_pointers.map(p => p.y));
    console.log('bounder', left_bounder, right_bounder, top_bounder, bottom_bounder);

    if (showingPrimitive.apiEndpoint !== 'Circle' && showingPrimitive.apiEndpoint !== 'Fill' && showingPrimitive.apiEndpoint !== 'Transform') {
      setShadowBounder({
        left_bounder,
        right_bounder,
        top_bounder,
        bottom_bounder,
      })
      const offset = BOUNDER_OFFSET;
      setShadowVertex([
        { x: left_bounder - offset, y: top_bounder - offset, type: 'bounder' },
        { x: right_bounder + offset, y: top_bounder - offset, type: 'bounder' },
        { x: right_bounder + offset, y: bottom_bounder + offset, type: 'bounder' },
        { x: left_bounder - offset, y: bottom_bounder + offset, type: 'bounder' },
        {
          x: (left_bounder + right_bounder) / 2,
          y: top_bounder - 2 * offset,
          type: 'rotate',
        },
        {
          x: Math.floor((left_bounder + right_bounder) / 2),
          y: Math.floor((top_bounder + bottom_bounder) / 2),
          type: 'center',
        }
      ])
    }
    else {
      setShadowBounder(null);
      setShadowVertex(null);
    }
  }, [showingPrimitive])

  const [translatingPrimitive, setTranslatingPrimitive] = React.useState<boolean>(false);
  const [originShadowBounder, setOriginShadowBounder] = React.useState<IShadowBounder | null>(null)
  const [originShadowVertex, setOriginShadowVertex] = React.useState<IPoint[] | null>(null);
  const [translateStartPoint, setTranslateStartPoint] = React.useState<IPoint | null>(null);
  const handleTranslateMouseDown = () => {
    setTranslatingPrimitive(true);
    setTranslateStartPoint(coordinate);
    setOriginShadowBounder(shadowBounder);
    setOriginShadowVertex(shadowVertex);
  }
  const handleTranslateMouseUp = () => {
    handleRotateMouseUp();
    if (!translateStartPoint || !originShadowBounder || !originShadowVertex) return;
    const offsetX = coordinate.x - translateStartPoint!.x;
    const offsetY = coordinate.y - translateStartPoint!.y;
    setTranslatingPrimitive(false);
    setOriginShadowBounder(null);
    setOriginShadowVertex(null);
    client('/engine/primitive/insert', {
      data: {
        Primitive: {
          Transform: {
            Translate: {
              offset: {
                x: offsetX,
                y: offsetY,
              }
            }
          }
        },
        Index: showingPrimitive?.index,
      }
    }).then(() => {
      fetchPrimitives(showingPrimitive?.index ? (showingPrimitive?.index + 1) : undefined);
    });
  }

  const [movingCenterPoint, setMovingCenterPoint] = React.useState<boolean>(false);
  const handleCenterMouseDown = () => {
    if (!showingPrimitive) return;
    setMovingCenterPoint(true);
  }
  const handleCenterMouseUp = () => {
    handleRotateMouseUp();
    if (!movingCenterPoint) return;
    setMovingCenterPoint(false);
  }

  const [movingRotatePoint, setMovingRotatePoint] = React.useState<boolean>(false);
  const [angle, setAngle] = React.useState(0);
  const [rotateStartPoint, setRotateStartPoint] = React.useState<IPoint | null>(null);
  const handleRotateMouseDown = (point: IPoint) => {
    setMovingRotatePoint(true);
    setRotateStartPoint(point);
  }
  const handleRotateMouseUp = () => {
    if (movingRotatePoint && rotateStartPoint) {
      const center_point = shadowVertex?.find(point => point.type === 'center');
      if (!center_point) return;
      client('/engine/primitive/insert', {
        data: {
          Primitive: {
            Transform: {
              Rotate: {
                angle,
                center: {
                  x: center_point.x,
                  y: center_point.y,
                }
              }
            }
          },
          Index: showingPrimitive?.index,
        }
      }).then(() => {
        fetchPrimitives(showingPrimitive?.index ? (showingPrimitive?.index + 1) : undefined);
      });
    }
    setRotateStartPoint(null);
    setMovingRotatePoint(false);
  }

  const [movingScalePoint, setMovingScalePoint] = React.useState<boolean>(false);
  const [originScalePoint, setOriginScalePoint] = React.useState<IPoint | null>(null);
  const getBounderPointType = (point: IPoint) => {
    if (!shadowBounder || !originShadowBounder) return 'unknown';
    const referenceBounder = originShadowBounder || shadowBounder;
    const offset = BOUNDER_OFFSET;
    if (point.x + offset === referenceBounder.left_bounder && point.y + offset === referenceBounder.top_bounder) return 'top-left';
    if (point.x - offset === referenceBounder.right_bounder && point.y + offset === referenceBounder.top_bounder) return 'top-right';
    if (point.x - offset === referenceBounder.right_bounder && point.y - offset === referenceBounder.bottom_bounder) return 'bottom-right';
    if (point.x + offset === referenceBounder.left_bounder && point.y - offset === referenceBounder.bottom_bounder) return 'bottom-left';
    return 'unknown';
  }
  const handleScalePointMouseDown = (point: IPoint) => {
    setMovingScalePoint(true);
    setOriginScalePoint(point);
    setOriginShadowBounder(shadowBounder);
  }
  const handleScalePointMouseUp = () => {
    handleRotateMouseUp();
    if (!movingScalePoint || !originScalePoint) return;
    const center_point = shadowVertex?.find(point => point.type === 'center');
    if (!center_point) return;
    const offsetX = coordinate.x - center_point.x;
    const offsetY = coordinate.y - center_point.y;
    const rawOffsetX = originScalePoint.x - center_point.x;
    const rawOffsetY = originScalePoint.y - center_point.y;
    const scalex = offsetX / rawOffsetX;
    const scaleY = offsetY / rawOffsetY;
    setMovingScalePoint(false);
    setOriginScalePoint(null);
    setOriginShadowBounder(null);
    client('/engine/primitive/insert', {
      data: {
        Primitive: {
          Transform: {
            Scale: {
              scale: {
                x: scalex,
                y: scaleY,
              },
              center: {
                x: center_point.x,
                y: center_point.y,
              }
            }
          }
        },
        Index: showingPrimitive?.index,
      }
    }).then(() => {
      fetchPrimitives(showingPrimitive?.index ? (showingPrimitive?.index + 1) : undefined);
    })
  }

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
    <div id="primitives">
      <div className='title'>历史记录</div>
      <div className='primitives' id="primitives-list">
        {
          primitives.map((primitive, index) => {
            return (
              <button key={index}
                className={`primitive${showingPrimitive?.index === primitive.index ? ' showing' : ''}`}
                onClick={() => {
                  if (showingPrimitive?.index === primitive.index)
                    setShowingPrimitive(null)
                  else
                    setShowingPrimitive(primitive)
                }}
              >
                {primitive.apiEndpoint}
              </button>
            )
          })
        }
      </div>
    </div>
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
      {
        shadowBounder && (
          <div
            id="shadowBounder"
            style={{
              left: shadowBounder.left_bounder,
              top: shadowBounder.top_bounder,
              width: shadowBounder.right_bounder - shadowBounder.left_bounder,
              height: shadowBounder.bottom_bounder - shadowBounder.top_bounder,
              transform: movingRotatePoint ? `rotate(${angle}rad)` : undefined,
              transformOrigin: `${shadowVertex!.find(point => point.type === 'center')!.x - shadowBounder.left_bounder}px ${shadowVertex!.find(point => point.type === 'center')!.y - shadowBounder.top_bounder}px`,
            }}
            onMouseMove={handleMouseMove}
            onMouseDown={handleTranslateMouseDown}
            onMouseUp={handleTranslateMouseUp}
          />
        )
      }
    </div>
    <div id="drawFuncs-wrapper">
      <div id="drawFuncs"
        style={{
          height: ((hideToolbar || disableToolbar) && toolbarHeight) ? 0 : toolbarHeight,
          padding: (hideToolbar || disableToolbar) ? '0 4px' : undefined,
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
      <button
        style={{ display: disableToolbar ? 'none' : undefined }}
        id="toolbar-control"
        onClick={() => {
          if (disableToolbar) return;
          setHideToolbar(!hideToolbar);
        }}>
        {hideToolbar ? '展开' : '收起'}工具栏
      </button>
    </div>
    {
      showingPrimitive && (
        <div id="show-primitive" style={{ opacity: primitiveOpacity }}>
          <div className='endpoint'>
            {showingPrimitive.apiEndpoint}
          </div>
          {
            showingPrimitive.params.map((param, index) => {
              // if (param.type === 'unknown') return null;
              if (param.value === undefined) return null;
              return (
                <div className='param' key={index}>
                  <div className='name'>{param.name}</div>
                  <div className='value'>
                    {
                      param.type === 'point' ? `(${param.value.x}, ${param.value.y})` :
                        param.type === 'multi_points' ? param.value.map((point: IPoint, index: number) => (
                          <div key={index}>
                            {`(${point.x}, ${point.y})`}
                          </div>
                        )) :
                          (typeof param.value === 'object') ? Object.entries(param.value).map(([key, value]) => (typeof value === 'object' && value !== null) ? (
                            <div className='param' key={key}>
                              <div className='name'>{key}</div>
                              <div className='value'>
                                {
                                  Object.entries(value).map(([k, v]) => (
                                    <div key={k}>
                                      {`${k}: ${v}`}
                                    </div>
                                  ))
                                }
                              </div>
                            </div>
                          ) : (
                            <div key={key}>
                              {`${key}: ${JSON.stringify(value)}`}
                            </div>
                          )) : JSON.stringify(param.value)
                    }
                  </div>
                </div>
              )
            })
          }
          <div className='actions'>
            <button onClick={() => {
              client("/engine/primitive/remove", {
                data: {
                  Index: showingPrimitive.index,
                }
              }).then(() => {
                setShowingPrimitive(null);
                fetchPrimitives()
              });
            }}>删除</button>
          </div>
        </div>
      )
    }
    {
      (!showingPrimitive
        ?
        ([...clickedPoints, { ...slidingWindowMoving ? [] : coordinate, type: clickingSlidingWindowPoints ? 'sliding' : dragging ? 'drag' : (currentDrawFunc.current.multiplePoints && clickedPoints.length >= currentDrawFunc.current.requiredPointers && getPointDistance(coordinate, clickedPoints[0]) < 10) ? 'ending' : 'current' }] as IPoint[]).filter(point => point.x && point.y)
        :
        [...showingPrimitive.params.map(param => {
          if (param.value === undefined) return [];
          if (param.type === 'multi_points') return param.value;
          if (param.type !== 'point') return [];
          return [param.value];
        }).filter(p => p.length > 0).flat().map(p => ({ ...p, type: 'dragable' })),
        ...shadowVertex ? shadowVertex : [],
        {
          ...(movingPrimitivePoint || movingCenterPoint || movingScalePoint) ? [] : { ...coordinate, type: 'view' }
        }].filter(point => point.x && point.y) as IPoint[]
      ).map((point, index) => {
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
              <div className={`point-circle ${getBounderPointType(point)}`}
                style={{
                  backgroundColor: point.type === 'view' ? 'green' : (point.type === 'sliding' || point.type === 'ending' || point.type === 'bounder' || point.type === 'center') ? 'transparent' : point.type === 'drag' ? 'yellow' : point.type === 'current' ? 'blue' : point.type === "rotate" ? 'yellow' : 'red'
                }}
                onMouseDown={point.type === 'dragable' ? () => handleEditPointMouseDown(point) : point.type === 'center' ? handleCenterMouseDown : point.type === 'rotate' ? () => handleRotateMouseDown(point) : point.type === 'bounder' ? () => handleScalePointMouseDown(point) : () => { }}
                onMouseUp={point.type === 'dragable' ? handleEditPointMouseUp : point.type === 'center' ? handleCenterMouseUp : point.type === 'rotate' ? handleRotateMouseUp : point.type === 'bounder' ? handleScalePointMouseUp : handleRotateMouseUp}
                onMouseMove={handleMouseMove}
              />
              <p className='point-text top'>
                {
                  (point.type === 'view' || point.type === 'dragable' || point.type === 'bounder' || point.type === "center" || point.type === "rotate") ? '' : clickingSlidingWindowPoints ? (index === 0 ? 'top-left' : 'bottom-right') :
                    point.type === 'ending' ? '结束' : (!currentDrawFunc.current.multiplePoints && (index + 1) === currentDrawFunc.current.requiredPointers) ? '结束' : (index + 1)
                }
              </p>
              <p className='point-text'>
                {
                  (point.type === 'view') ? movingRotatePoint ? `${(angle * (-180 / Math.PI)).toFixed(2)}°` : '' :
                    (point.type === 'bounder' || point.type === 'center' || point.type === 'rotate')
                      ? '' : `(${point.x}, ${point.y})`
                }
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
