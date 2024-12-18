import * as React from 'react';
import { backend_endpoint, engine_name, engine_fps, client, retry_max_times } from '../../utils/client';
import drawFuncs from '../../utils/drawFuncs';

import './playground.css';
import { BOUNDER_OFFSET, IDrawApiParam, IGlobalOptions, IPoint, IPrimitive, IShadowBounder } from '../../utils/types';
import { randomEngineName } from '../../utils';

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

async function QueueDo(funcs: Array<() => Promise<void>>): Promise<void> {
  for (const func of funcs) {
    await func();
  }
}

function getPointDistance(p1: IPoint, p2: IPoint) {
  return Math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2);
}

export default function PlayGround() {
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

    if (movingKnotPoint && shadowVertex && shadowBounder) {
      const currentVertexes = [...shadowVertex];
      const currentKnot = currentVertexes.find(point => point.type === 'knot' && point.index === movingKnotPoint);
      const currentKnots = currentVertexes.filter(point => point.type === 'knot');
      if (!currentKnot) return;
      const current_control_points = showingPrimitive?.params.find(param => param.type === 'multi_points')?.value as IPoint[];
      const control_points_length = current_control_points?.length || 0;
      const knots_length = currentKnots.length;
      const order = knots_length - 1 - control_points_length;
      const index = currentKnot.index || 0;
      const minKnot = currentKnots[0].value || 0;
      const maxKnot = currentKnots[currentKnots.length - 1].value || 1;
      const prevKnot = (index === 0 ? 0 : currentKnots[index - 1].value) || 0;
      const specialPrevKnot = index === control_points_length ? (currentKnots[order].value || 0) + 0.25 : 0;
      const nextKnot = (index < currentKnots.length - 1 ? currentKnots[index + 1].value : 114514) || 114514;
      const specialNextKnot = index === order ? (currentKnots[control_points_length].value || 0.25) - 0.25 : 114514;
      const usePrevKnot = Math.max(prevKnot, specialPrevKnot);
      const useNextKnot = Math.min(nextKnot, specialNextKnot);
      const newKnot = (coordinateRef.current.x - shadowBounder.left_bounder) / (shadowBounder.right_bounder - shadowBounder.left_bounder) * (maxKnot - minKnot) + minKnot;
      const fixedKnot = Math.min(useNextKnot, Math.max(usePrevKnot, newKnot));
      currentKnot.value = fixedKnot;
      setShadowVertex(currentVertexes.map(p => p.type === 'knot' ? ({
        x: shadowBounder.left_bounder + (shadowBounder.right_bounder - shadowBounder.left_bounder) * ((p.value || 0) / (maxKnot - minKnot)),
        y: p.y,
        type: p.type,
        index: p.index,
        value: p.value,
      }) : p));
    } else if (movingScalePoint && originScalePoint && originShadowBounder) {
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
      const calculateAngleBetweenPoints = (center: IPoint, p1: IPoint, p2: IPoint): number => {
        const v1 = { x: p1.x - center.x, y: p1.y - center.y };
        const v2 = { x: p2.x - center.x, y: p2.y - center.y };
        const dotProduct = v1.x * v2.x + v1.y * v2.y;
        const magnitudeV1 = Math.sqrt(v1.x * v1.x + v1.y * v1.y);
        const magnitudeV2 = Math.sqrt(v2.x * v2.x + v2.y * v2.y);
        const cosTheta = dotProduct / (magnitudeV1 * magnitudeV2);
        let angleInRadians = Math.acos(cosTheta);
        const crossProduct = v1.x * v2.y - v1.y * v2.x;
        if (crossProduct < 0) {
          angleInRadians = -angleInRadians;
        }
        return angleInRadians;
      }
      const center_point = shadowVertex?.find(point => point.type === 'center');
      if (!center_point) return;
      setAngle(calculateAngleBetweenPoints(center_point, rotateStartPoint, coordinate));
    } else if ((showingPrimitive || showingPrimitives.length > 0) && movingCenterPoint && shadowVertex) {
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
    if (clickingSlidingWindowPoints && slidingWindowMode === 'Rectangle') {
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
    } else if (clickingSlidingWindowPoints && slidingWindowMode === 'Polygon') {
      setClickingSlidingWindowPoints(false);
      setClickedPoints([]);
      slidingWindowPolygonPointsRef.current = pointers;
      setSlidingWindowPolygonPoints(pointers);
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
      else if (clickingSlidingWindowPoints && slidingWindowMode === 'Polygon') return;
      else {
        clickedPointsRef.current.push({ x: e.clientX, y: e.clientY });
        setClickedPoints(clickedPointsRef.current);
      }
    }
    if (showingPrimitive) {
      return;
    } else if (clickingSlidingWindowPoints) {
      // 处理圈画裁剪框的点击
      if (slidingWindowMode === 'Rectangle' && clickedPointsRef.current.length === 2) {
        handleDraw();
      } else {
        // 多边形裁剪框的点击在 handleClick 中处理
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
    // handleClick 仅处理 click 类型的绘制方法，drag 类型的绘制方法、圈画矩形裁剪框和修改图元的已有点在 handleMouseDown 中处理
    if (showingPrimitive) return;
    if (clickingSlidingWindowPoints && slidingWindowMode === 'Rectangle') return;
    if (currentDrawFunc.current.drawingMethod === 'drag') return;
    if ((currentDrawFunc.current.multiplePoints && clickedPointsRef.current.length >= currentDrawFunc.current.requiredPointers || clickingSlidingWindowPoints && slidingWindowMode === 'Polygon' && clickedPointsRef.current.length >= 3) && getPointDistance(coordinate, clickedPointsRef.current[0]) < 10) {
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
    console.log('Sliding window changed', enabledSliding, slidingWindowMode, slidingWindowAlgorithm);
    if (slidingWindowMode === 'Rectangle') {
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
      setGlobalOptions({
        ...globalOptions,
        clip: {
          enable: enabledSliding,
          window: {
            Rectangle: currentSlidingWindow,
          },
          algorithm: slidingWindowAlgorithm,
        },
      })
    } else if (slidingWindowMode === 'Polygon') {
      setGlobalOptions({
        ...globalOptions,
        clip: {
          enable: enabledSliding,
          window: {
            Polygon: {
              points: slidingWindowPolygonPointsRef.current,
            }
          },
          algorithm: slidingWindowAlgorithm,
        },
      })
    }
  }

  const [hideToolbar, setHideToolbar] = React.useState(false);
  const [disableToolbar, setDisableToolbar] = React.useState(false);
  const [toolbarHeight, setToolbarHeight] = React.useState(0);

  const previewSlidingWindowTimeout = React.useRef<number | null>(null);

  const [primitives, setPrimitives] = React.useState<IPrimitive[]>([]);
  const [showingPrimitive, setShowingPrimitive] = React.useState<IPrimitive | null>(null);
  const [showingPrimitives, setShowingPrimitives] = React.useState<IPrimitive[]>([]);
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
            else if (param.type === 'knots')
              return param.name || `knots`;
            else return param.name || `u${index + 1}`;
          }),
          algorithm: df.drawFunc.algorithm,
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
              else if (param.type === 'knots')
                return param.name || `knots`;
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
        return { apiEndpoint, params, index, type: matchedDrawFunc?.type || 'unknown', editable: matchedDrawFunc ? true : false, algorithm: matchedDrawFunc?.algorithm || 0 };
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
    if (showingPrimitive === null && showingPrimitives.length === 0) {
      setDisableToolbar(false);
      setPrimitiveOpacity(0);
    } else {
      setDisableToolbar(true);
      setPrimitiveOpacity(1);
    }
  }, [showingPrimitive, showingPrimitives])
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
    if (showingPrimitives.length !== 0) return;
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

    const average_x = Math.floor(all_pointers.reduce((sum, p) => sum + p.x, 0) / all_pointers.length);
    const average_y = Math.floor(all_pointers.reduce((sum, p) => sum + p.y, 0) / all_pointers.length);

    if (showingPrimitive.apiEndpoint !== 'Circle' && showingPrimitive.apiEndpoint !== 'Fill' && showingPrimitive.apiEndpoint !== 'Transform') {
      setShadowBounder({
        left_bounder,
        right_bounder,
        top_bounder,
        bottom_bounder,
      })
      const offset = BOUNDER_OFFSET;

      let knots_point: IPoint[] = [];

      if (showingPrimitive.params.find(param => param.type === 'knots')) {
        const knots = showingPrimitive.params.find(param => param.type === 'knots')?.value as number[];
        knots_point = knots.map((knot, index) => (
          {
            x: left_bounder + (right_bounder - left_bounder) * (knot / (knots[knots.length - 1] - knots[0])),
            y: top_bounder - 3 * offset,
            type: 'knot',
            value: knot,
            index,
          }
        ))
      }

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
          x: average_x,
          y: average_y,
          type: 'center',
        },
        ...knots_point,
      ])
    }
    else {
      setShadowBounder(null);
      setShadowVertex(null);
    }
  }, [showingPrimitive, showingPrimitives])
  React.useEffect(() => {
    if (showingPrimitive) return;
    if (showingPrimitives.length === 0) {
      setShadowBounder(null);
      setShadowVertex(null);
      return;
    }
    const all_pointers: IPoint[] = showingPrimitives.map(primitive => primitive.params.map(param => (param.type === 'point' && param.value) ? ({ ...param.value, type: 'bounder' }) : undefined).filter(p => !!p) as IPoint[]).flat();
    const param_multi_points = showingPrimitives.map(primitive => primitive.params.find(param => param.type === 'multi_points')).filter(p => !!p);
    if (param_multi_points.length > 0)
      all_pointers.push(...param_multi_points.map(param => (param.value as IPoint[]).map(point => ({ ...point, type: 'bounder' }) as IPoint)).flat());
    console.log('all_pointers', all_pointers);

    const left_bounder: number = Math.min(...all_pointers.map(p => p.x));
    const right_bounder: number = Math.max(...all_pointers.map(p => p.x));
    const top_bounder: number = Math.min(...all_pointers.map(p => p.y));
    const bottom_bounder: number = Math.max(...all_pointers.map(p => p.y));
    console.log('bounder', left_bounder, right_bounder, top_bounder, bottom_bounder);

    const average_x = Math.floor(all_pointers.reduce((sum, p) => sum + p.x, 0) / all_pointers.length);
    const average_y = Math.floor(all_pointers.reduce((sum, p) => sum + p.y, 0) / all_pointers.length);

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
        x: average_x,
        y: average_y,
        type: 'center',
      }
    ])
  }, [showingPrimitive, showingPrimitives])

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
    if (showingPrimitive)
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
    else if (showingPrimitives.length > 0) {
      QueueDo(showingPrimitives.sort((a, b) => b.index - a.index).map(primitive => () => new Promise((resolve) => {
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
            Index: primitive.index,
          }
        }).then(() => resolve());
      }))).then(() => {
        setShowingPrimitives([]);
        fetchPrimitives();
      });
    }
  }

  const [movingCenterPoint, setMovingCenterPoint] = React.useState<boolean>(false);
  const handleCenterMouseDown = () => {
    setMovingCenterPoint(true);
  }
  const handleCenterMouseUp = () => {
    handleRotateMouseUp();
    if (!movingCenterPoint) return;
    setMovingCenterPoint(false);
  }

  const [movingKnotPoint, setMovingKnotPoint] = React.useState<false | number>(false);
  const hanldeKnotPointMouseDown = (index?: number) => {
    if (index === undefined) return;
    setMovingKnotPoint(index);
  }

  const [movingRotatePoint, setMovingRotatePoint] = React.useState<boolean>(false);
  const [angle, setAngle] = React.useState(0);
  const [rotateStartPoint, setRotateStartPoint] = React.useState<IPoint | null>(null);
  const handleRotateMouseDown = (point: IPoint) => {
    setMovingRotatePoint(true);
    setRotateStartPoint(point);
  }
  const handleRotateMouseUp = () => {
    if (movingKnotPoint !== false) {
      const currentKnot = shadowVertex?.find(point => point.type === 'knot' && point.index === movingKnotPoint);
      if (showingPrimitive && currentKnot && currentKnot.index !== undefined && currentKnot.value !== undefined) {
        console.log('moved knot point', currentKnot);
        const newKnots = showingPrimitive?.params.find(param => param.type === 'knots')?.value as number[];
        newKnots[currentKnot.index] = currentKnot.value;
        client('/engine/primitive/modify', {
          data: {
            Primitive: {
              [showingPrimitive.apiEndpoint || "unknown"]: {
                type: showingPrimitive.type,
                ...Object.fromEntries((showingPrimitive.params || []).map((param) => [param.name || 'unknown', param.value])),
                knots: newKnots,
                algorithm: 0,
              }
            },
            Index: showingPrimitive.index,
          }
        })
      }
      setMovingKnotPoint(false);
    }
    if (movingRotatePoint && rotateStartPoint) {
      const center_point = shadowVertex?.find(point => point.type === 'center');
      if (!center_point) return;
      if (showingPrimitive)
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
      else if (showingPrimitives.length > 0)
        QueueDo(showingPrimitives.sort((a, b) => b.index - a.index).map(primitive => () => new Promise((resolve) => {
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
              Index: primitive.index,
            }
          }).then(() => resolve());
        }))).then(() => {
          setShowingPrimitives([]);
          fetchPrimitives();
        })
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
    if (showingPrimitive)
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
    else if (showingPrimitives.length > 0)
      QueueDo(showingPrimitives.sort((a, b) => b.index - a.index).map(primitive => () => new Promise((resolve) => {
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
            Index: primitive.index,
          }
        }).then(() => resolve());
      }))).then(() => {
        setShowingPrimitives([]);
        fetchPrimitives();
      })
  }

  const [slidingWindowMode, setSlidingWindowMode] = React.useState<'Rectangle' | 'Polygon'>('Rectangle');
  const [slidingWindowAlgorithm, setSlidingWindowAlgorithm] = React.useState<0 | 1>(0);
  const slidingWindowPolygonPointsRef = React.useRef<IPoint[]>([]);
  const [slidingWindowPolygonPoints, setSlidingWindowPolygonPoints] = React.useState<IPoint[]>([]);
  const [globalOptions, setGlobalOptions] = React.useState<IGlobalOptions>({
    background_color: {
      r: 0,
      g: 0,
      b: 0,
      a: 255,
    },
    clip: {
      enable: false,
      window: {
        Rectangle: {
          top_left: { x: 0, y: 0 },
          bottom_right: { x: 0, y: 0 },
        },
      },
      algorithm: 0,
    },
  });
  React.useEffect(() => {
    if ('Rectangle' in globalOptions.clip.window) {
      // 长方形裁剪框
      setSlidingWindow(globalOptions.clip.window.Rectangle);
    } else {
      // 多边形裁剪框
    }
    setEnableSlidingWindow(globalOptions.clip.enable);
    client('/engine/set_global_options', {
      data: {
        GlobalOptions: globalOptions,
      }
    })
  }, [globalOptions])

  React.useEffect(() => {
    if (hideToolbar || disableToolbar) return;
    setToolbarHeight(getHeightUnfold((document.getElementById('drawFuncs')!)) - 8);
  }, [hideToolbar, disableToolbar, penOptions, enableSlidingWindow, slidingWindowMode])

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
                className={`primitive${(showingPrimitive?.index === primitive.index || showingPrimitives.find(i => i.index === primitive.index)) ? ' showing' : ''}`}
                onClick={(e: React.MouseEvent<HTMLButtonElement>) => {
                  if (!e.metaKey && !e.ctrlKey) {
                    setShowingPrimitives([]);
                    if (showingPrimitive?.index === primitive.index)
                      setShowingPrimitive(null)
                    else
                      setShowingPrimitive(primitive)
                  }
                  else {
                    // 多选的逻辑
                    console.log('Ctrl clicked');
                    const oldShowingPrimitive = showingPrimitive;
                    const currentPrimitives = [...showingPrimitives];
                    if (oldShowingPrimitive) {
                      setShowingPrimitive(null)
                      currentPrimitives.push(oldShowingPrimitive);
                    }
                    if (showingPrimitives.find(p => p.index === primitive.index)) {
                      setShowingPrimitives(currentPrimitives.filter(p => p.index !== primitive.index));
                    } else {
                      setShowingPrimitives([...currentPrimitives, primitive]);
                    }
                  }
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
        (((enableSlidingWindow && slidingWindowMode === 'Rectangle') ? [
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
        ] : (enableSlidingWindow && slidingWindowMode === 'Polygon') ? slidingWindowPolygonPoints : []) as (IPoint & { name: string })[]).map((point, index) => {
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
                      if (slidingWindowMode === 'Rectangle') {
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
                      } else {
                        const currentSlidingWindow = slidingWindowPolygonPointsRef.current;
                        currentSlidingWindow[index] = { x: e.clientX, y: e.clientY };
                        setSlidingWindowPolygonPoints(currentSlidingWindow);
                      }
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
        display: (enableSlidingWindow && slidingWindowMode === 'Rectangle') ? 'block' : 'none',
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
              setSlidingWindowMode('Rectangle');
              setClickingSlidingWindowPoints(true)
            }}>
              圈画矩形裁剪框
            </button>
          )
        }
        {
          enableSlidingWindow && (
            <button onClick={() => {
              setClickedPoints([]);
              clickedPointsRef.current = [];
              setSlidingWindowMode('Polygon');
              setClickingSlidingWindowPoints(true)
            }}>
              圈画多边形裁剪框
            </button>
          )
        }
        {
          enableSlidingWindow && slidingWindowMode === 'Rectangle' && (
            <div className="option">
              裁剪算法
              <select
                value={globalOptions.clip.algorithm}
                onChange={e => {
                  setSlidingWindowAlgorithm(parseInt(e.target.value) as 0 | 1);
                  const currentGlobalOptions = { ...globalOptions };
                  currentGlobalOptions.clip.algorithm = parseInt(e.target.value) as 0 | 1;
                  setGlobalOptions(currentGlobalOptions);
                }
                }>
                <option value={0}>CS</option>
                <option value={1}>Midpoint</option>
              </select>
            </div>
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
                        )) : param.type === 'knots' ? ((arr: number[], size: number): number[][] => {
                          const result: number[][] = [];
                          for (let i = 0; i < arr.length; i += size) {
                            result.push(arr.slice(i, i + size));
                          }
                          return result;
                        })(param.value, 2).map(a => <div className='value'>
                          {(a.map((v: number) => v.toFixed(2)).join(', '))}
                        </div>) :
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
      (showingPrimitives.length > 0) && (
        <div id="show-primitive" style={{ opacity: primitiveOpacity }}>
          <div className='endpoint'>
            多个选中的图元
          </div>
          {
            showingPrimitives.map((primitive, index) => {
              return (
                <div key={index} className='param'>
                  <div className='name'>{primitive.apiEndpoint}</div>
                  <div className='value'>
                    {
                      primitive.params.map((param, index) => {
                        if (param.value === undefined) return null;
                        return (
                          <div key={index}>
                            {param.name}: {JSON.stringify(param.value)}
                          </div>
                        )
                      })
                    }
                  </div>
                </div>
              )
            })
          }

          <div className='actions'>
            <button onClick={() => {
              QueueDo(showingPrimitives.sort((a, b) => b.index - a.index).map(primitive => () => {
                return new Promise((resolve, reject) => {
                  client("/engine/primitive/remove", {
                    data: {
                      Index: primitive.index,
                    }
                  }).then(() => {
                    resolve();
                  }).catch(reject);
                })
              })).then(() => {
                setShowingPrimitives([]);
                fetchPrimitives()
              });
            }}>删除全部</button>
          </div>
        </div>
      )
    }
    {
      (
        showingPrimitives.length !== 0 ? [
          ...shadowVertex ? shadowVertex : [],
          ...(movingPrimitivePoint || movingCenterPoint || movingScalePoint) ? [] : [{ ...coordinate, type: 'view' }]
        ].filter(point => point.x && point.y) as IPoint[] :
          !showingPrimitive
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
                  backgroundColor: point.type === 'view' ? 'green' : (point.type === 'sliding' || point.type === 'ending' || point.type === 'bounder' || point.type === 'center') ? 'transparent' : (point.type === 'drag') ? 'yellow' : point.type === 'current' ? 'blue' : point.type === "rotate" ? 'yellow' : point.type === 'knot' ? (
                    point.index === showingPrimitive?.params.find(param => param.type === 'multi_points')?.value?.length ? '#2ecc71' :
                      point.index === showingPrimitive?.params.find(param => param.type === 'knots')?.value?.length - 1 - showingPrimitive?.params.find(param => param.type === 'multi_points')?.value?.length ? '#2ecc71' :
                        '#d35400'
                  ) : 'red'
                }}
                onMouseDown={point.type === 'dragable' ? () => handleEditPointMouseDown(point) : point.type === 'center' ? handleCenterMouseDown : point.type === 'rotate' ? () => handleRotateMouseDown(point) : point.type === 'bounder' ? () => handleScalePointMouseDown(point) : point.type === 'knot' ? () => hanldeKnotPointMouseDown(point.index) : () => { }}
                onMouseUp={point.type === 'dragable' ? handleEditPointMouseUp : point.type === 'center' ? handleCenterMouseUp : point.type === 'rotate' ? handleRotateMouseUp : point.type === 'bounder' ? handleScalePointMouseUp : handleRotateMouseUp}
                onMouseMove={handleMouseMove}
              />
              <p className='point-text top'>
                {
                  (point.type === 'view' || point.type === 'dragable' || point.type === 'bounder' || point.type === "center" || point.type === "rotate" || point.type === "knot") ? '' : clickingSlidingWindowPoints ? slidingWindowMode === 'Rectangle' ? (index === 0 ? 'top-left' : 'bottom-right') : (index + 1) :
                    point.type === 'ending' ? '结束' : (!currentDrawFunc.current.multiplePoints && (index + 1) === currentDrawFunc.current.requiredPointers) ? '结束' : (index + 1)
                }
              </p>
              <p className='point-text'>
                {
                  (point.type === 'view') ? movingRotatePoint ? `${(angle * (-180 / Math.PI)).toFixed(2)}°` : '' :
                    (point.type === 'bounder' || point.type === 'center' || point.type === 'rotate' || point.type === 'knot')
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
