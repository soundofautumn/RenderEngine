import { client } from "./client";
import { IDrawApiParam, IDrawFuncParams, IPoint } from "./types";


class DrawFunc {
  public readonly requiredPointers: number;
  public readonly apiEndpoint: string;
  public readonly drawingMethod: 'click' | 'drag';
  public readonly multiplePoints: boolean = false;
  public readonly params: IDrawApiParam[];
  public readonly type: string | undefined;
  public readonly algorithm: number = 0;

  constructor(props: { params: IDrawApiParam[], requiredPointers: number, apiEndpoint: string, drawingMethod?: 'click' | 'drag', type?: string, multiplePoints?: boolean, algorithm?: number }) {
    this.params = props.params.sort((a) => a.type === 'point' ? -1 : 1);
    this.requiredPointers = props.requiredPointers;
    this.apiEndpoint = props.apiEndpoint;
    this.drawingMethod = props.drawingMethod || 'drag';
    this.type = props.type;
    this.multiplePoints = props.multiplePoints || false;
    this.algorithm = props.algorithm || 0;

    if (this.requiredPointers < this.params.filter(param => param.type === 'point').length)
      throw new Error("Illegal number of required pointers");
    if (this.params.some(param => param.type === 'func' && (!param.func || !param.name)))
      throw new Error("Function parameter missing function or name");
    if (this.params.filter(param => param.type === 'point').length !== 2 && this.drawingMethod === 'drag')
      throw new Error("Drag drawing method only supports 2 points");
    if (this.multiplePoints && this.params.filter(param => param.type === 'multi_points').length !== 1)
      throw new Error("Multiple points drawing method requires multi_points");
  }

  public draw(props: IDrawFuncParams): Promise<void> {
    const { pointers } = props;
    return new Promise((resolve, reject) => {
      if (pointers.length < this.requiredPointers) {
        reject("Not enough pointers");
      }
      client("/engine/primitive/push_back", {
        data: {
          [this.apiEndpoint]: {
            type: this.type,
            ...Object.fromEntries(this.params.map((param, index) => {
              if (param.type === 'point')
                return [param.name || `p${index + 1}`, pointers[index]];
              else if (param.type === 'func' && param.func)
                return [param.name || `f${index + 1}`, param.func(...pointers)];
              else if (param.type === 'multi_points')
                return [param.name || `mp${index + 1}`, pointers];
              else return [param.name || `u${index + 1}`, null];
            })),
            algorithm: this.algorithm,
          }
        }
      }).then(() => {
        resolve();
      }).catch(e => {
        console.error(e);
        reject("Failed to draw");
      })
    });
  }
}

interface IDrawFunc {
  name: string;
  drawFunc: DrawFunc;
}

const drawFuncs: IDrawFunc[] = []

drawFuncs.push({
  name: '点画线 DDA',
  drawFunc: new DrawFunc(
    {
      params: [
        {
          type: 'point',
        },
        {
          type: 'point',
        },
      ],
      requiredPointers: 2,
      apiEndpoint: 'Line',
      drawingMethod: 'click',
      algorithm: 0,
    }
  ),
})

drawFuncs.push({
  name: '点画线 MidP',
  drawFunc: new DrawFunc(
    {
      params: [
        {
          type: 'point',
        },
        {
          type: 'point',
        },
      ],
      requiredPointers: 2,
      apiEndpoint: 'Line',
      drawingMethod: 'click',
      algorithm: 1,
    }
  ),
})

drawFuncs.push({
  name: '点画线 Bresenham',
  drawFunc: new DrawFunc(
    {
      params: [
        {
          type: 'point',
        },
        {
          type: 'point',
        },
      ],
      requiredPointers: 2,
      apiEndpoint: 'Line',
      drawingMethod: 'click',
      algorithm: 2,
    }
  ),
})

drawFuncs.push({
  name: '点画圆',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'point',
        name: 'point_on_circle',
      },
      {
        type: 'point',
        name: 'center'
      },
      {
        type: 'func',
        name: 'radius',
        func: (point_on_circle, center) => Math.floor(Math.sqrt((point_on_circle.x - center.x) ** 2 + (point_on_circle.y - center.y) ** 2)),
      }
    ],
    requiredPointers: 2,
    apiEndpoint: 'Circle',
    drawingMethod: 'click',
    type: 'center_radius',
  }),
})

drawFuncs.push({
  name: '拖画圆',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'point',
        name: 'point_on_circle',
      },
      {
        type: 'point',
        name: 'center'
      },
      {
        type: 'func',
        name: 'radius',
        func: (point_on_circle, center) => Math.floor(Math.sqrt((point_on_circle.x - center.x) ** 2 + (point_on_circle.y - center.y) ** 2)),
      }
    ],
    requiredPointers: 2,
    apiEndpoint: 'Circle',
    drawingMethod: 'drag',
    type: 'center_radius',
  }),
})

drawFuncs.push({
  name: '三点画圆',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'point',
      },
      {
        type: 'point',
      },
      {
        type: 'point',
      },
    ],
    requiredPointers: 3,
    apiEndpoint: 'Circle',
    drawingMethod: 'click',
    type: 'three_points',
  }),
})

drawFuncs.push({
  name: '三点画弧',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'point',
      },
      {
        type: 'point',
      },
      {
        type: 'point',
      },
    ],
    requiredPointers: 3,
    apiEndpoint: 'Arc',
    drawingMethod: 'click',
    type: 'three_points',
  }),
})

drawFuncs.push({
  name: '长方形',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'func',
        name: 'top_left',
        func: (a: IPoint, b: IPoint) => {
          return {
            x: Math.min(a.x, b.x),
            y: Math.min(a.y, b.y)
          }
        }
      },
      {
        type: 'func',
        name: 'bottom_right',
        func: (a: IPoint, b: IPoint) => {
          return {
            x: Math.max(a.x, b.x),
            y: Math.max(a.y, b.y)
          }
        }
      },
    ],
    requiredPointers: 2,
    apiEndpoint: 'Rectangle',
    drawingMethod: 'click',
  }),
})

drawFuncs.push({
  name: '多边形',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'multi_points',
        name: 'points',
      }
    ],
    requiredPointers: 3,
    apiEndpoint: 'Polygon',
    drawingMethod: 'click',
    multiplePoints: true,
  }),
})

drawFuncs.push({
  name: '贝塞尔曲线',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'multi_points',
        name: 'control_points',
      }
    ],
    requiredPointers: 3,
    apiEndpoint: 'BezierCurve',
    drawingMethod: 'click',
    multiplePoints: true,
  }),
})

drawFuncs.push({
  name: 'B样条曲线',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'multi_points',
        name: 'control_points',
      },
      {
        type: 'func',
        name: 'knots',
        func: (...control_points) => {
          return Array.from({ length: control_points.length * 2 }, (_, i) => parseFloat(prompt(`第 ${i + 1} 个结点值`) || '0') + 0.01);
        }
      }
    ],
    requiredPointers: 3,
    apiEndpoint: 'BsplineCurve',
    drawingMethod: 'click',
    multiplePoints: true,
  }),
})

drawFuncs.push({
  name: '种子填充',
  drawFunc: new DrawFunc({
    params: [
      {
        type: 'point',
        name: 'seed',
      }
    ],
    requiredPointers: 1,
    apiEndpoint: 'Fill',
    drawingMethod: 'click',
  }),
})

export default drawFuncs;
