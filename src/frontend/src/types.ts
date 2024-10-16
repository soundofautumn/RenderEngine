export interface IPoint {
    x: number;
    y: number;
    type?:
        | "click"
        | "current"
        | "drag"
        | "sliding"
        | "ending"
        | "dragable"
        | "view"
        | "bounder"
        | "center"
        | "rotate";
}

export interface IDrawFuncParams {
    pointers: IPoint[];
    color?: string;
    algorithm?: number;
}

export type IDrawApiParamType = "point" | "func" | "multi_points" | "unknown";

export interface IDrawApiParam {
    type: IDrawApiParamType;
    name?: string;
    func?: (...pointers: IPoint[]) => number | object;
}

export interface IPrimitive {
    apiEndpoint: string;
    params: (IDrawApiParam & {
        value: any;
    })[];
    index: number;
    editable: boolean;
    type?: string;
    algorithm: number;
}

export interface IShadowBounder {
    left_bounder: number;
    right_bounder: number;
    top_bounder: number;
    bottom_bounder: number;
}

export const BOUNDER_OFFSET = 10;
