import { client } from "./client";
import { IPoint } from "./types";

interface IDrawFuncParams {
  pointers: IPoint[];
  color?: string;
  algorithm?: number;
}

interface IDrawApiParam {
  type: 'point' | 'func';
  name?: string;
  func?: (...pointers: IPoint[]) => number;
}

class DrawFunc {
  public readonly requiredPointers: number;
  public readonly apiEndpoint: string;
  public readonly drawingMethod: 'click' | 'drag';

  private readonly params: IDrawApiParam[];
  private readonly type: string | undefined;

  constructor(props: { params: IDrawApiParam[], requiredPointers: number, apiEndpoint: string, drawingMethod?: 'click' | 'drag', type?: string }) {
    this.params = props.params.sort((a) => a.type === 'point' ? -1 : 1);
    this.requiredPointers = props.requiredPointers;
    this.apiEndpoint = props.apiEndpoint;
    this.drawingMethod = props.drawingMethod || 'drag';
    this.type = props.type;

    if (this.requiredPointers < this.params.filter(param => param.type === 'point').length)
      throw new Error("Illegal number of required pointers");
    if (this.params.some(param => param.type === 'func' && (!param.func || !param.name)))
      throw new Error("Function parameter missing function or name");
    if (this.params.filter(param => param.type === 'point').length !== 2 && this.drawingMethod === 'drag')
      throw new Error("Drag drawing method only supports 2 points");
  }

  public draw(props: IDrawFuncParams): Promise<void> {
    const { pointers, algorithm = 0 } = props;
    return new Promise((resolve, reject) => {
      if (pointers.length < this.requiredPointers) {
        reject("Not enough pointers");
      }
      client("/engine/draw", {
        data: {
          [this.apiEndpoint]: {
            type: this.type,
            ...Object.fromEntries(this.params.map((param, index) => {
              if (param.type === 'point')
                return [param.name || `p${index + 1}`, pointers[index]];
              else if (param.type === 'func' && param.func)
                return [param.name || `f${index + 1}`, param.func(...pointers)];
              else return [param.name || `u${index + 1}`, null];
            })),
            algorithm,
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
  name: 'Click to Draw Line',
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
    }
  ),
})

drawFuncs.push({
  name: 'Drag to Draw Line',
  drawFunc: new DrawFunc({
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
    drawingMethod: 'drag',
  }),
})

drawFuncs.push({
  name: 'Click to Circle',
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
  name: 'Drag to Circle',
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
  name: 'Circle with Three Points',
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
  name: 'Arc with Three Points',
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

export default drawFuncs;
